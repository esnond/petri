#ifndef CE_METRICS_H
#define CE_METRICS_H

#include <stdio.h>
#include "modulators.h"

typedef struct {
    double t_s;
    float  speed_ema;      /* mm/s */
    float  odometer_mm;    /* actual head path length */
    float  prev_head_x;    /* previous head position for odometer */
    float  prev_head_y;
    int    have_prev_head;
    long   n_reversals;
    float  energy_cost;    /* body integrated muscle work proxy */
    long   n_omega;
    float  rev_len_acc;    /* in body lengths */
    float  t_near_odor;    /* s */
    float  t_near_ctrl;    /* s */
    float  chemotaxis_index;
    /* reversal detector state */
    int    in_reversal;
    float  rev_start_t;
    float  rev_dist_mm;
    /* omega detector state */
    float  heading_prev;
    float  heading_acc;
    float  omega_window_t;
    int    have_prev;
} metrics_t;

void metrics_init(metrics_t* mt);
void metrics_step(metrics_t* mt, const body_t* b, const nervous_t* ns,
                  const modulators_t* m, const world_t* w, float dt_ms);
void metrics_csv_header(FILE* out);
void metrics_csv_row(FILE* out, const metrics_t* mt, const body_t* b,
                     const nervous_t* ns, const modulators_t* m);

#endif
