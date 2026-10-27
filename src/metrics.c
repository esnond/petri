#include <math.h>
#include <stdio.h>
#include <string.h>
#include "metrics.h"
#include "config.h"

void metrics_init(metrics_t* mt)
{
    memset(mt, 0, sizeof(*mt));
    mt->prev_head_x = 0.0f;
    mt->prev_head_y = 0.0f;
    mt->have_prev_head = 0;
}

void metrics_step(metrics_t* mt, const body_t* b, const nervous_t* ns,
                  const modulators_t* m, const world_t* w, float dt_ms)
{
    mt->energy_cost = b->energy_cost;
    float dt_s = dt_ms * 0.001f;
    float dx = b->x[0] - b->x[1];
    float dy = b->y[0] - b->y[1];
    float da, dctrl;
    float head_dx, head_dy, head_disp;
    (void)ns;
    (void)m;
    (void)w;
    (void)dx;
    (void)dy;

    mt->t_s += dt_s;

    /* odometer accumulates real head displacement, not joint distance */
    if (!mt->have_prev_head) {
        mt->prev_head_x = b->x[0];
        mt->prev_head_y = b->y[0];
        mt->have_prev_head = 1;
    } else {
        head_dx = b->x[0] - mt->prev_head_x;
        head_dy = b->y[0] - mt->prev_head_y;
        head_disp = hypotf(head_dx, head_dy);
        mt->odometer_mm += head_disp;
        mt->prev_head_x = b->x[0];
        mt->prev_head_y = b->y[0];
    }

    mt->speed_ema += 0.02f * (b->speed - mt->speed_ema);

    /* reversal episode detection, signed speed below threshold for 0.5 s */
    if (b->speed < -0.02f) {
        if (!mt->in_reversal) {
            mt->in_reversal = 1;
            mt->rev_start_t = (float)mt->t_s;
            mt->rev_dist_mm = 0.0f;
        }
        mt->rev_dist_mm += -b->speed * dt_s;
    } else if (mt->in_reversal) {
        if (mt->t_s - mt->rev_start_t > 0.5) {
            mt->n_reversals++;
            mt->rev_len_acc += mt->rev_dist_mm / BODY_LEN_MM;
        }
        mt->in_reversal = 0;
    }

    /* omega turns, deep body bend plus almost a full heading sweep in 1 s */
    {
        float heading = b->head_angle;
        float dh;
        if (!mt->have_prev) {
            mt->heading_prev = heading;
            mt->have_prev = 1;
        }
        dh = heading - mt->heading_prev;
        if (dh > 3.14159265f) dh -= 6.2831853f;
        if (dh < -3.14159265f) dh += 6.2831853f;
        mt->heading_acc += dh;
        mt->heading_prev = heading;
        mt->omega_window_t += dt_s;
        if (mt->omega_window_t >= 1.0f) {
            float kmax = 0.0f;
            int i;
            for (i = 0; i < N_SEG; i++) {
                if (fabsf(b->k[i]) > kmax)
                    kmax = fabsf(b->k[i]);
            }
            if (fabsf(mt->heading_acc) > 2.6f && kmax > 7.0f)
                mt->n_omega++;
            mt->heading_acc = 0.0f;
            mt->omega_window_t = 0.0f;
        }
    }

    /* chemotaxis index accumulators, time near each source */
    da = hypotf(b->x[0] - ODOR_A_X_MM, b->y[0] - ODOR_A_Y_MM);
    dctrl = hypotf(b->x[0] - ODOR_B_X_MM, b->y[0] - ODOR_B_Y_MM);
    if (da < ZONE_R_MM)
        mt->t_near_odor += dt_s;
    if (dctrl < ZONE_R_MM)
        mt->t_near_ctrl += dt_s;
    {
        float tot = mt->t_near_odor + mt->t_near_ctrl;
        mt->chemotaxis_index = tot > 1.0f
            ? (mt->t_near_odor - mt->t_near_ctrl) / tot
            : 0.0f;
    }
}

void metrics_csv_header(FILE* out)
{
    fprintf(out, "t_s,vel_mm_s,curv_inv_mm,k_head,k_tail,ava,avb,aib,aiy,serotonin,head_x_mm,head_y_mm\n");
}

void metrics_csv_row(FILE* out, const metrics_t* mt, const body_t* b,
                     const nervous_t* ns, const modulators_t* m)
{
    int ava = nervous_find_prefix(ns, "AVA");
    int avb = nervous_find_prefix(ns, "AVB");
    int aib = nervous_find_prefix(ns, "AIB");
    int aiy = nervous_find_prefix(ns, "AIY");
    float kmean = 0.0f;
    int i;
    for (i = 0; i < N_SEG; i++)
        kmean += b->k[i];
    kmean /= (float)N_SEG;

    fprintf(out, "%.2f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.2f,%.2f\n",
            mt->t_s,
            b->speed,
            kmean,
            b->k[N_SEG / 4],
            b->k[3 * N_SEG / 4],
            ava >= 0 ? neuron_act(&ns->cells[ava]) : 0.0f,
            avb >= 0 ? neuron_act(&ns->cells[avb]) : 0.0f,
            aib >= 0 ? neuron_act(&ns->cells[aib]) : 0.0f,
            aiy >= 0 ? neuron_act(&ns->cells[aiy]) : 0.0f,
            m->ser,
            b->x[0], b->y[0]);
}
