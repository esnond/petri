#ifndef CE_MODULATORS_H
#define CE_MODULATORS_H

#include "world.h"

typedef struct {
    float ser;       /* serotonin, 0..1, rises on food */
    float oct;       /* octopamine, 0..1, rises off food, drives roaming */
    float dop;       /* dopamine, 0..1, rises on mechanical contact */
    float ser_t, oct_t, dop_t;   /* targets set by the environment */
    float mech_trace;           /* decaying trace of mechanical stimulation */
} modulators_t;

void modulators_init(modulators_t* m);
void modulators_step(modulators_t* m, const world_t* w, const body_t* b, float dt_ms);

#endif
