#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "tui.h"
#include "world.h"
#include "config.h"

/*
 * ascii codes below 128 pass straight through the glyph grid, anything
 * above is an index into this table. utf8 lives only here, one place.
 */
static const char* const GLYPH[] = {
    "\xe2\x94\x80",             /* 128 horizontal line */
    "\xe2\x94\x82",             /* 129 vertical line */
    "\xe2\x95\xad",             /* 130 round corner nw */
    "\xe2\x95\xae",             /* 131 round corner ne */
    "\xe2\x95\xb0",             /* 132 round corner sw */
    "\xe2\x95\xaf",             /* 133 round corner se */
    "\xe2\x96\x81",             /* 134 bar 1/8 */
    "\xe2\x96\x82",             /* 135 bar 2/8 */
    "\xe2\x96\x83",             /* 136 bar 3/8 */
    "\xe2\x96\x84",             /* 137 bar 4/8 */
    "\xe2\x96\x85",             /* 138 bar 5/8 */
    "\xe2\x96\x86",             /* 139 bar 6/8 */
    "\xe2\x96\x87",             /* 140 bar 7/8 */
    "\xe2\x96\x88",             /* 141 full block */
    "\xe2\x96\x8f",             /* 142 left 1/8 */
    "\xe2\x96\x8e",             /* 143 left 2/8 */
    "\xe2\x96\x8d",             /* 144 left 3/8 */
    "\xe2\x96\x8c",             /* 145 left 4/8 */
    "\xe2\x96\x8b",             /* 146 left 5/8 */
    "\xe2\x96\x8a",             /* 147 left 6/8 */
    "\xe2\x96\x89",             /* 148 left 7/8 */
    "\xe2\x97\x8f",             /* 149 filled circle */
};
enum { G_LINE = 128, G_PIPE, G_NW, G_NE, G_SW, G_SE,
       G_B1, G_B2, G_B3, G_B4, G_B5, G_B6, G_B7, G_BF,
       G_L1, G_L2, G_L3, G_L4, G_L5, G_L6, G_L7, G_DOT };

/* curvature ramp for the kymograph, magnitude picks the char */
static const unsigned char RAMP[10] = {
    ' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'
};

enum { K_RESET, K_GREEN, K_BGREEN, K_MAGENTA, K_BMAGENTA, K_YELLOW,
       K_CYAN, K_BCYAN, K_BLUE, K_DIM, K_WHITE, K_RED, K_COUNT };

static const char* const COL_CODE[K_COUNT] = {
    "\x1b[0m", "\x1b[32m", "\x1b[92m", "\x1b[35m", "\x1b[95m", "\x1b[33m",
    "\x1b[36m", "\x1b[96m", "\x1b[34m", "\x1b[90m", "\x1b[97m", "\x1b[91m"
};

static struct termios g_tio_saved;
static int g_tio_raw = 0;
static int g_stdin_flags = -1;

/* the glyph grid is the whole screen, colors and characters */
static unsigned char g_grid[TUI_ROWS][TUI_COLS];
static unsigned char g_col[TUI_ROWS][TUI_COLS];
static char g_frame[TUI_ROWS * TUI_COLS * 12];

static int g_rows = 24, g_cols = 80;

static void grid_clear(void)
{
    memset(g_grid, ' ', sizeof(g_grid));
    memset(g_col, 0, sizeof(g_grid));
}

static void cell(int x, int y, int g, int k)
{
    if (x < 0 || y < 0 || x >= TUI_COLS || y >= TUI_ROWS)
        return;
    g_grid[y][x] = (unsigned char)g;
    g_col[y][x] = (unsigned char)k;
}

static void text(int x, int y, const char* s, int k)
{
    while (*s) {
        cell(x, y, (unsigned char)*s, k);
        x++;
        s++;
    }
}

static void box(int x, int y, int w, int h, const char* title)
{
    int i;
    cell(x, y, G_NW, K_DIM);
    cell(x + w - 1, y, G_NE, K_DIM);
    cell(x, y + h - 1, G_SW, K_DIM);
    cell(x + w - 1, y + h - 1, G_SE, K_DIM);
    for (i = 1; i < w - 1; i++) {
        cell(x + i, y, G_LINE, K_DIM);
        cell(x + i, y + h - 1, G_LINE, K_DIM);
    }
    for (i = 1; i < h - 1; i++) {
        cell(x, y + i, G_PIPE, K_DIM);
        cell(x + w - 1, y + i, G_PIPE, K_DIM);
    }
    if (title && title[0])
        text(x + 2, y, title, K_WHITE);
}

/* horizontal bar of width w cells from a 0..1 value */
static void bar(int x, int y, int w, float v01, int k)
{
    float n;
    int i, full;
    float frac;
    int frac_g = 0;
    if (v01 < 0.0f) v01 = 0.0f;
    if (v01 > 1.0f) v01 = 1.0f;
    n = v01 * (float)w;
    full = (int)n;
    frac = n - (float)full;
    if (frac >= 0.875f) frac_g = G_L7;
    else if (frac >= 0.75f) frac_g = G_L6;
    else if (frac >= 0.625f) frac_g = G_L5;
    else if (frac >= 0.5f) frac_g = G_L4;
    else if (frac >= 0.375f) frac_g = G_L3;
    else if (frac >= 0.25f) frac_g = G_L2;
    else if (frac >= 0.125f) frac_g = G_L1;
    for (i = 0; i < w; i++)
        cell(x + i, y, i < full ? G_BF : ' ', k);
    if (frac_g && full < w)
        cell(x + full, y, frac_g, k);
}

/* one row bipolar sparkline, newest sample at the right edge, blank
   cells until the history fills */
static void spark_row(int x, int y, int w, const float* v, int n)
{
    int i;
    for (i = 0; i < w; i++) {
        int back = w - 1 - i;
        float val = 0.0f;
        int lvl;
        if (back < n)
            val = v[n - 1 - back];
        lvl = (int)(fabsf(val) * 7.99f);
        if (lvl > 7) lvl = 7;
        if (back >= n)
            cell(x + i, y, ' ', K_RESET);
        else
            cell(x + i, y, G_B1 + lvl, val >= 0.0f ? K_BGREEN : K_BMAGENTA);
    }
}

static float cell_act(const sim_t* s, int idx)
{
    return idx >= 0 ? neuron_act(&s->ns.cells[idx]) : 0.0f;
}

/* push a new kymograph row when enough simulated time passed */
static void kym_update(tui_t* t, const sim_t* s)
{
    int i;
    if ((float)s->metrics.t_s - t->kym_last_t < TUI_KYM_DT_MS / 1000.0f)
        return;
    t->kym_last_t = (float)s->metrics.t_s;
    for (i = 0; i < N_SEG; i++)
        t->kym[t->kym_head][i] = s->body.k[i];
    t->kym_head = (t->kym_head + 1) % TUI_KYM_ROWS;
    if (t->kym_fill < TUI_KYM_ROWS)
        t->kym_fill++;
}

static void draw_world(tui_t* t, sim_t* s, int sel, int ox, int oy, int w, int h)
{
    int x, y, i;
    (void)t;
    for (y = 0; y < h && y < WORLD_H; y++) {
        for (x = 0; x < w && x < WORLD_W; x++) {
            float mmx = ((float)x + 0.5f) * WORLD_CELL_MM;
            float mmy = ((float)y + 0.5f) * WORLD_CELL_MM;
            float da = hypotf(mmx - ODOR_A_X_MM, mmy - ODOR_A_Y_MM);
            float db = hypotf(mmx - ODOR_B_X_MM, mmy - ODOR_B_Y_MM);
            int rim = (x == 0 || y == 0 || x == WORLD_W - 1 || y == WORLD_H - 1);
            if (rim) {
                cell(ox + x, oy + y, '#', K_DIM);
            } else if (da < 1.0f) {
                cell(ox + x, oy + y, 'O', K_BCYAN);
            } else if (db < 1.0f) {
                cell(ox + x, oy + y, 'C', K_BLUE);
            } else if (s->world.enable_food && s->world.food[y][x] > 0.35f &&
                       (x + y) % 2 == 0) {
                cell(ox + x, oy + y, ',', K_YELLOW);
            } else {
                cell(ox + x, oy + y, ' ', K_RESET);
            }
        }
    }
    if (sel >= 0 && sel < s->ns.n) {
        /* the selected neuron glows on its soma position along the body,
           motor neurons sit on the worm, others have no body position */
    }
    /* the worm, colored by local bend: dorsal green, ventral magenta */
    for (i = 0; i < N_SEG; i++) {
        int ax = ox + (int)(s->body.x[i] / WORLD_CELL_MM);
        int ay = oy + (int)(s->body.y[i] / WORLD_CELL_MM);
        int bx = ox + (int)(s->body.x[i + 1] / WORLD_CELL_MM);
        int by = oy + (int)(s->body.y[i + 1] / WORLD_CELL_MM);
        /* bresenham, integer only */
        int dx = abs(bx - ax), sx = ax < bx ? 1 : -1;
        int dy = -abs(by - ay), sy = ay < by ? 1 : -1;
        int err = dx + dy;
        int kcol = s->body.k[i] > 0.15f ? K_BGREEN :
                   (s->body.k[i] < -0.15f ? K_BMAGENTA : K_GREEN);
        int g = (i == 0) ? G_DOT : (i == N_SEG - 1 ? '~' : 'o');
        for (;;) {
            int e2;
            cell(ax, ay, g, kcol);
            if (ax == bx && ay == by)
                break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; ax += sx; }
            if (e2 <= dx) { err += dx; ay += sy; }
        }
    }
}

static void draw_side(tui_t* t, sim_t* s, int paused, float speed_mult, int sel,
                      int ox, int oy, int w, int h)
{
    int r = 1;
    char buf[64];
    (void)t;

    snprintf(buf, sizeof(buf), "%s", paused ? "PAUSED" : "running");
    text(ox + 2, oy + r, buf, paused ? K_YELLOW : K_BGREEN);
    snprintf(buf, sizeof(buf), "x%.1f  seed %lu",
             (double)speed_mult, (unsigned long)s->opts.seed);
    text(ox + w - 2 - (int)strlen(buf), oy + r, buf, K_DIM);
    r += 2;

    text(ox + 2, oy + r, "t", K_DIM);
    snprintf(buf, sizeof(buf), "%7.1f s", (double)s->metrics.t_s);
    text(ox + 8, oy + r, buf, K_WHITE);
    r++;
    text(ox + 2, oy + r, "speed", K_DIM);
    snprintf(buf, sizeof(buf), "%6.3f mm/s", (double)s->body.speed);
    text(ox + 8, oy + r, buf, s->body.speed > 0.05f ? K_BGREEN : K_WHITE);
    r++;
    text(ox + 2, oy + r, "path", K_DIM);
    snprintf(buf, sizeof(buf), "%7.2f mm", (double)s->metrics.odometer_mm);
    text(ox + 8, oy + r, buf, K_WHITE);
    r++;
    text(ox + 2, oy + r, "ci", K_DIM);
    snprintf(buf, sizeof(buf), "%+7.2f", (double)s->metrics.chemotaxis_index);
    text(ox + 8, oy + r, buf, s->metrics.chemotaxis_index > 0.0f ? K_BCYAN : K_WHITE);
    r++;
    text(ox + 2, oy + r, "bouts", K_DIM);
    snprintf(buf, sizeof(buf), "rev %ld  omega %ld",
             s->metrics.n_reversals, s->metrics.n_omega);
    text(ox + 8, oy + r, buf, K_WHITE);
    r += 2;

    text(ox + 2, oy + r, "neural", K_WHITE);
    r++;
    text(ox + 2, oy + r, "ava", K_DIM);
    bar(ox + 7, oy + r, 18, cell_act(s, s->idx_ava), K_BMAGENTA);
    r++;
    text(ox + 2, oy + r, "avb", K_DIM);
    bar(ox + 7, oy + r, 18, cell_act(s, s->idx_avb), K_BGREEN);
    r++;
    text(ox + 2, oy + r, "aib", K_DIM);
    bar(ox + 7, oy + r, 18, cell_act(s, s->idx_aib), K_CYAN);
    r++;
    text(ox + 2, oy + r, "aiy", K_DIM);
    bar(ox + 7, oy + r, 18, cell_act(s, s->idx_aiy), K_CYAN);
    r += 2;

    text(ox + 2, oy + r, "modulators", K_WHITE);
    r++;
    text(ox + 2, oy + r, "5ht", K_DIM);
    bar(ox + 7, oy + r, 18, s->mod.ser, K_BCYAN);
    r++;
    text(ox + 2, oy + r, "da", K_DIM);
    bar(ox + 7, oy + r, 18, s->mod.dop, K_YELLOW);
    r++;
    text(ox + 2, oy + r, "oct", K_DIM);
    bar(ox + 7, oy + r, 18, s->mod.oct, K_BMAGENTA);
    r += 2;

    text(ox + 2, oy + r, s->world.enable_food ? "food lawn on" : "food lawn off",
         s->world.enable_food ? K_YELLOW : K_DIM);
    if (r + 2 < h) {
        r += 2;
        text(ox + 2, oy + r, "surgery", K_WHITE);
        r++;
        if (sel >= 0 && sel < s->ns.n) {
            char nm[32];
            int cut = sim_surgery_iscut(s, sel);
            snprintf(nm, sizeof(nm), "%-6s %s", s->ns.cells[sel].name,
                     cut ? "CUT" : "intact");
            text(ox + 2, oy + r, nm, cut ? K_RED : K_BGREEN);
            r++;
            bar(ox + 2, oy + r, 18, cell_act(s, sel), K_BCYAN);
            r++;
        }
        snprintf(buf, sizeof(buf), "%d cut", s->n_surgery);
        text(ox + 2, oy + r, buf, s->n_surgery ? K_RED : K_DIM);
        r++;
    }
    if (r + 3 < h) {
        r++;
        text(ox + 2, oy + r, "q p t f +/-", K_DIM);
        r++;
        text(ox + 2, oy + r, ",/. neuron", K_DIM);
        r++;
        text(ox + 2, oy + r, "enter cut x undo", K_DIM);
        r++;
        text(ox + 2, oy + r, "w save session", K_DIM);
    }
}

static void draw_kym(tui_t* t, int ox, int oy, int w, int h)
{
    int r, i, j;
    char buf[80];
    snprintf(buf, sizeof(buf), "curvature kymograph, one row per %.0f ms, head left",
             (double)TUI_KYM_DT_MS);
    text(ox + 2, oy, buf, K_WHITE);
    (void)w;
    for (r = 0; r < TUI_KYM_ROWS; r++) {
        int row = ((t->kym_head - t->kym_fill + r) % TUI_KYM_ROWS + TUI_KYM_ROWS)
                  % TUI_KYM_ROWS;
        int have = r >= TUI_KYM_ROWS - t->kym_fill;
        for (i = 0; i < N_SEG && i * 3 + 3 < w - 2; i++) {
            float k = have ? t->kym[row][i] : 0.0f;
            int lvl = (int)(fabsf(k) * 9.0f / 9.0f);
            int g, kk;
            if (lvl > 9) lvl = 9;
            g = lvl == 0 ? ' ' : RAMP[lvl];
            kk = k > 0.0f ? K_BGREEN : (k < 0.0f ? K_BMAGENTA : K_DIM);
            for (j = 0; j < 3; j++)
                cell(ox + 2 + i * 3 + j, oy + 1 + r, g, kk);
        }
    }
    text(ox + 2, oy + TUI_KYM_ROWS + 1, "head", K_DIM);
    text(ox + 6 + N_SEG * 3 - 4, oy + TUI_KYM_ROWS + 1, "tail", K_DIM);
    if (h > TUI_KYM_ROWS + 3)
        spark_row(ox + 2, oy + TUI_KYM_ROWS + 2, w - 4, t->spark, t->spark_fill);
}

void tui_render(tui_t* t, sim_t* s, int paused, float speed_mult, int sel)
{
    size_t fp = 0;
    int x, y, last;
    float kv;

    grid_clear();

    /* head curvature history for the sparkline, one sample per frame */
    kv = s->body.k[N_SEG / 4];
    if (t->spark_fill < TUI_SPARK_N)
        t->spark_fill++;
    for (x = t->spark_fill - 1; x > 0; x--)
        t->spark[x] = t->spark[x - 1];
    t->spark[0] = kv;
    kym_update(t, s);

    if (t->compact) {
        box(0, 0, WORLD_W + 2, WORLD_H + 2, "petri");
        draw_world(t, s, sel, 1, 1, WORLD_W, WORLD_H);
        snprintf(g_frame, sizeof(g_frame),
                 "\x1b[H\x1b[2J");
        fp = strlen(g_frame);
        /* world rows, then one status line */
        for (y = 0; y < WORLD_H + 2; y++) {
            last = K_RESET;
            for (x = 0; x < WORLD_W + 2; x++) {
                int k = g_col[y][x];
                if (k != last) {
                    fp += (size_t)snprintf(g_frame + fp, sizeof(g_frame) - fp,
                                           "%s", COL_CODE[k]);
                    last = k;
                }
                {
                    int g = g_grid[y][x];
                    const char* gs = g >= 128 ? GLYPH[g - 128] : NULL;
                    if (gs) {
                        size_t n = strlen(gs);
                        if (fp + n < sizeof(g_frame) - 1) {
                            memcpy(g_frame + fp, gs, n);
                            fp += n;
                        }
                    } else {
                        g_frame[fp++] = (char)g;
                    }
                }
            }
            g_frame[fp++] = '\n';
        }
        fp += (size_t)snprintf(g_frame + fp, sizeof(g_frame) - fp,
                "%s t %6.1f  spd %6.3f  %s  q p t +/- \x1b[0m\n",
                paused ? "\x1b[33mPAUSE" : "",
                (double)s->metrics.t_s, (double)s->body.speed,
                s->world.enable_food ? "food" : "    ");
        fwrite(g_frame, 1, fp, stdout);
        fflush(stdout);
        return;
    }

    /* full dashboard: world panel, state panel, kymograph panel.
       exact height, 34 world rows plus 12 kymograph rows */
    box(0, 0, WORLD_W + 2, WORLD_H + 2, "petri, c elegans connectome");
    draw_world(t, s, sel, 1, 1, WORLD_W, WORLD_H);
    box(WORLD_W + 2, 0, TUI_COLS - WORLD_W - 2, WORLD_H + 2, "state");
    draw_side(t, s, paused, speed_mult, sel, WORLD_W + 2, 0,
              TUI_COLS - WORLD_W - 2, WORLD_H + 2);
    box(0, WORLD_H + 2, TUI_COLS, TUI_KYM_ROWS + 4, NULL);
    draw_kym(t, 0, WORLD_H + 3, TUI_COLS - 2, TUI_KYM_ROWS + 3);

    {
        int rows = WORLD_H + 2 + TUI_KYM_ROWS + 4;
        fp += (size_t)snprintf(g_frame, sizeof(g_frame), "\x1b[H");
        for (y = 0; y < rows; y++) {
            last = K_RESET;
            for (x = 0; x < TUI_COLS; x++) {
                int k = g_col[y][x];
                int g = g_grid[y][x];
                const char* gs = g >= 128 ? GLYPH[g - 128] : NULL;
                if (k != last) {
                    fp += (size_t)snprintf(g_frame + fp, sizeof(g_frame) - fp,
                                           "%s", COL_CODE[k]);
                    last = k;
                }
                if (gs) {
                    size_t n = strlen(gs);
                    if (fp + n < sizeof(g_frame) - 1) {
                        memcpy(g_frame + fp, gs, n);
                        fp += n;
                    }
                } else {
                    g_frame[fp++] = (char)g;
                }
            }
            g_frame[fp++] = '\n';
        }
    }
    fp += (size_t)snprintf(g_frame + fp, sizeof(g_frame) - fp, "\x1b[0m");
    fwrite(g_frame, 1, fp, stdout);
    fflush(stdout);
}

static void term_size(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0) {
        g_rows = ws.ws_row;
        g_cols = ws.ws_col;
    }
}

/* the dashboard needs 46 rows by 80 columns */
int tui_wants_compact(void)
{
    term_size();
    return g_rows < WORLD_H + 2 + TUI_KYM_ROWS + 5 || g_cols < TUI_COLS - 3;
}

int tui_enter(void)
{
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &g_tio_saved) != 0)
        return -1;
    term_size();
    t = g_tio_saved;
    t.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &t) != 0)
        return -1;
    g_tio_raw = 1;
    if (write(STDOUT_FILENO, "\x1b[?1049h\x1b[?25l", 14) < 0)
        return -1;
    return 0;
}

void tui_leave(void)
{
    if (g_tio_raw) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_tio_saved);
        /* best effort restore at exit, nothing to recover if the tty
           is already gone, but the result must be consumed explicitly
           because ubuntu glibc marks write warn_unused_result */
        ssize_t wn = write(STDOUT_FILENO, "\x1b[0m\x1b[?25h\x1b[?1049l", 18);
        (void)wn;
        g_tio_raw = 0;
    }
    if (g_stdin_flags >= 0) {
        fcntl(STDIN_FILENO, F_SETFL, g_stdin_flags);
        g_stdin_flags = -1;
    }
}

/* called from main before the loop, nonblocking stdin belongs here too */
int tui_stdin_nonblock(void)
{
    g_stdin_flags = fcntl(STDIN_FILENO, F_GETFL);
    if (g_stdin_flags == -1)
        return -1;
    if (fcntl(STDIN_FILENO, F_SETFL, g_stdin_flags | O_NONBLOCK) == -1)
        return -1;
    return 0;
}
