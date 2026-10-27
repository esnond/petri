#ifndef CE_TUI_H
#define CE_TUI_H

#include "sim.h"

/* terminal dashboard, hand rolled ansi, no curses. the frame is composed
   into a static glyph grid and flushed with one write, so the screen never
   tears and the main loop never allocates */

#define TUI_ROWS 48
#define TUI_COLS 84
#define TUI_KYM_ROWS 8
#define TUI_KYM_DT_MS 150.0f   /* sim time per kymograph row */
#define TUI_SPARK_N 64

typedef struct {
    int   compact;             /* terminal too small for the full dashboard */
    float kym_last_t;          /* sim time the newest kymograph row was cut */
    int   kym_head;            /* ring write position */
    int   kym_fill;
    float kym[TUI_KYM_ROWS][N_SEG];
    float spark[TUI_SPARK_N];  /* recent head curvature, newest last */
    int   spark_fill;
} tui_t;

int  tui_enter(void);        /* raw mode, alt screen, 0 on success */
int  tui_stdin_nonblock(void);
int  tui_wants_compact(void); /* terminal too small for the dashboard */
void tui_leave(void);        /* restore terminal, always safe */
void tui_render(tui_t* t, sim_t* s, int paused, float speed_mult, int sel);

#endif
