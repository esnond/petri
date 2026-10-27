#ifndef CE_WORLD_H
#define CE_WORLD_H

#include "body.h"

#define WORLD_W 48
#define WORLD_H 32
#define WORLD_CELL_MM 0.5f
#define WORLD_W_MM (WORLD_W * WORLD_CELL_MM)
#define WORLD_H_MM (WORLD_H * WORLD_CELL_MM)

/* plate landmarks, mm */
static const float ODOR_A_X_MM = 17.5f;
static const float ODOR_A_Y_MM = 8.0f;
static const float ODOR_B_X_MM = 5.5f;
static const float ODOR_B_Y_MM = 8.0f;
static const float FOOD_CX_MM  = 12.0f;
static const float FOOD_CY_MM  = 8.0f;
static const float FOOD_R_MM   = 5.0f;
static const float ZONE_R_MM   = 4.0f;   /* scoring radius around odor sources */

typedef struct {
    float odor_a[WORLD_H][WORLD_W];  /* attractant, arbitrary units */
    float odor_b[WORLD_H][WORLD_W];  /* control odor */
    float food[WORLD_H][WORLD_W];    /* lawn biomass 0..1 */
    float temp[WORLD_H][WORLD_W];    /* deg c */
    float ca_adapt, cb_adapt;        /* per receptor adaptation for awc and ase paths */
    float t_adapt;                   /* afd adaptation */
    float t_cult;                    /* cultivation temperature, deg c */
    int   diff_cnt;                  /* diffusion substep counter */
    int   enable_food;
    int   enable_thermo;
} world_t;

void world_init(world_t* w, rng_t* rng, int enable_food, int enable_thermo);
void world_step(world_t* w, body_t* b, float dt_ms);
void world_sample(const world_t* w, float x_mm, float y_mm,
                  float* ca, float* cb, float* food, float* temp);
float world_gradient_dc(const world_t* w, const body_t* b, int channel);

#endif
