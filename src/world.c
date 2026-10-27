#include <math.h>
#include <string.h>
#include "world.h"
#include "config.h"

/* ping pong buffer for the explicit 5 point diffusion stencil */
static float buf[WORLD_H][WORLD_W];

static int cell_ix(float mm)
{
    int i = (int)(mm / WORLD_CELL_MM);
    if (i < 0) i = 0;
    if (i > WORLD_W - 1) i = WORLD_W - 1;
    return i;
}

static int cell_iy(float mm)
{
    int i = (int)(mm / WORLD_CELL_MM);
    if (i < 0) i = 0;
    if (i > WORLD_H - 1) i = WORLD_H - 1;
    return i;
}

void world_sample(const world_t* w, float x_mm, float y_mm,
                  float* ca, float* cb, float* food, float* temp)
{
    int ix = cell_ix(x_mm), iy = cell_iy(y_mm);
    if (ca) *ca = w->odor_a[iy][ix];
    if (cb) *cb = w->odor_b[iy][ix];
    if (food) *food = w->food[iy][ix];
    if (temp) *temp = w->temp[iy][ix];
}

void world_init(world_t* w, rng_t* rng, int enable_food, int enable_thermo)
{
    int x, y;
    memset(w, 0, sizeof(*w));
    w->enable_food = enable_food;
    w->enable_thermo = enable_thermo;
    w->t_cult = 20.0f;
    w->ca_adapt = 0.0f;
    w->cb_adapt = 0.0f;
    w->t_adapt = w->t_cult;

    for (y = 0; y < WORLD_H; y++) {
        for (x = 0; x < WORLD_W; x++) {
            float cx = ((float)x + 0.5f) * WORLD_CELL_MM;
            float cy = ((float)y + 0.5f) * WORLD_CELL_MM;
            float da = hypotf(cx - ODOR_A_X_MM, cy - ODOR_A_Y_MM);
            float db = hypotf(cx - ODOR_B_X_MM, cy - ODOR_B_Y_MM);
            /* steady state gaussian around each source, diffusion length l */
            w->odor_a[y][x] = 10.0f * expf(-da * da / (2.0f * 6.0f * 6.0f));
            w->odor_b[y][x] = 10.0f * expf(-db * db / (2.0f * 6.0f * 6.0f));
            if (enable_food) {
                float df = hypotf(cx - FOOD_CX_MM, cy - FOOD_CY_MM);
                if (df < FOOD_R_MM)
                    w->food[y][x] = 1.0f - 0.6f * (df / FOOD_R_MM);
            }
            /* isothermal by default, linear gradient if thermo assay is on */
            if (enable_thermo)
                w->temp[y][x] = 17.0f + 6.0f * (cx / WORLD_W_MM);
            else
                w->temp[y][x] = w->t_cult + 0.1f * rng_uniform(rng);
        }
    }
    w->diff_cnt = 0;
}

static void diffuse_step(world_t* w, float dt_s)
{
    int x, y;
    float r = ODOR_DIFF_MM2_S * dt_s / (WORLD_CELL_MM * WORLD_CELL_MM);
    if (r > 0.2f)
        r = 0.2f;   /* stability guard for the explicit stencil */
    for (y = 0; y < WORLD_H; y++) {
        for (x = 0; x < WORLD_W; x++) {
            float c = w->odor_a[y][x];
            float l = x > 0 ? w->odor_a[y][x - 1] : c;
            float rt = x < WORLD_W - 1 ? w->odor_a[y][x + 1] : c;
            float u = y > 0 ? w->odor_a[y - 1][x] : c;
            float d = y < WORLD_H - 1 ? w->odor_a[y + 1][x] : c;
            buf[y][x] = c + r * (l + rt + u + d - 4.0f * c);
        }
    }
    memcpy(w->odor_a, buf, sizeof(buf));
    for (y = 0; y < WORLD_H; y++) {
        for (x = 0; x < WORLD_W; x++) {
            float c = w->odor_b[y][x];
            float l = x > 0 ? w->odor_b[y][x - 1] : c;
            float rt = x < WORLD_W - 1 ? w->odor_b[y][x + 1] : c;
            float u = y > 0 ? w->odor_b[y - 1][x] : c;
            float d = y < WORLD_H - 1 ? w->odor_b[y + 1][x] : c;
            buf[y][x] = c + r * (l + rt + u + d - 4.0f * c);
        }
    }
    memcpy(w->odor_b, buf, sizeof(buf));
}

static void replenish(world_t* w)
{
    /* the source spots are held fixed, a continuous supply assumption */
    int ax = cell_ix(ODOR_A_X_MM), ay = cell_iy(ODOR_A_Y_MM);
    int bx = cell_ix(ODOR_B_X_MM), by = cell_iy(ODOR_B_Y_MM);
    w->odor_a[ay][ax] = 10.0f;
    w->odor_b[by][bx] = 10.0f;
}

void world_step(world_t* w, body_t* b, float dt_ms)
{
    float dt_s = dt_ms * 0.001f;
    int headx = cell_ix(b->x[0]);
    int heady = cell_iy(b->y[0]);

    /* walls are the rim of the assay plate, the contact band is one cell
       wider than the hard stop so the touch sensors fire while the nose
       still has a hair of travel left */
    b->nose_contact = b->contact_hint;
    b->contact_hint = 0;
    b->nose_contact = b->nose_contact ||
        (b->x[0] < 2.0f * WORLD_CELL_MM || b->y[0] < 2.0f * WORLD_CELL_MM ||
         b->x[0] > WORLD_W_MM - 2.0f * WORLD_CELL_MM ||
         b->y[0] > WORLD_H_MM - 2.0f * WORLD_CELL_MM);
    b->tail_contact =
        (b->x[N_SEG] < 2.0f * WORLD_CELL_MM ||
         b->y[N_SEG] < 2.0f * WORLD_CELL_MM ||
         b->x[N_SEG] > WORLD_W_MM - 2.0f * WORLD_CELL_MM ||
         b->y[N_SEG] > WORLD_H_MM - 2.0f * WORLD_CELL_MM);

    /* diffusion every 10 ms, concentration changes slowly relative to dt */
    w->diff_cnt++;
    if (w->diff_cnt >= 10) {
        w->diff_cnt = 0;
        diffuse_step(w, 0.01f);
        replenish(w);
    }

    if (w->enable_food) {
        int x, y;
        /* grazing depletes the lawn at the head, the lawn regrows slowly */
        if (w->food[heady][headx] > 0.0f)
            w->food[heady][headx] -= FOOD_EAT_RATE * dt_s;
        if (w->food[heady][headx] < 0.0f)
            w->food[heady][headx] = 0.0f;
        for (y = 0; y < WORLD_H; y += 2)
            for (x = 0; x < WORLD_W; x += 2)
                if (w->food[y][x] < 1.0f)
                    w->food[y][x] += FOOD_REGEN_S * dt_s;
    }

    /* receptor adaptation runs on the head sample, first order */
    {
        float ca, cb, t;
        world_sample(w, b->x[0], b->y[0], &ca, &cb, NULL, &t);
        {
            float tau = CHEMO_TAU_ADAPT_S * 1000.0f;
            w->ca_adapt += (dt_ms / tau) * (ca - w->ca_adapt);
            w->cb_adapt += (dt_ms / tau) * (cb - w->cb_adapt);
            w->t_adapt += (dt_ms / (THERMO_TAU_ADAPT_S * 1000.0f)) * (t - w->t_adapt);
        }
    }
}

float world_gradient_dc(const world_t* w, const body_t* b, int channel)
{
    /* time derivative of concentration along the head trajectory */
    int ix = cell_ix(b->x[0]), iy = cell_iy(b->y[0]);
    const float (*f)[WORLD_W] = channel == 0 ? w->odor_a : w->odor_b;
    float cx = ((float)ix + 0.5f) * WORLD_CELL_MM;
    float cy = ((float)iy + 0.5f) * WORLD_CELL_MM;
    float vx = b->vx * 1000.0f, vy = b->vy * 1000.0f;   /* mm/s */
    int jx = cell_ix(cx + vx * 0.25f), jy = cell_iy(cy + vy * 0.25f);
    return (f[jy][jx] - f[iy][ix]) * 4.0f;   /* units per s */
}
