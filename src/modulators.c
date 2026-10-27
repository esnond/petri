#include <stddef.h>
#include "modulators.h"
#include "config.h"

void modulators_init(modulators_t* m)
{
    m->ser = 0.0f;
    m->oct = 0.0f;
    m->dop = 0.0f;
    m->ser_t = 0.0f;
    m->oct_t = 0.0f;
    m->dop_t = 0.0f;
    m->mech_trace = 0.0f;
}

/*
 * serotonin and octopamine are two halves of the feeding state, they bias
 * conductance and time constants continuously, no mode switch anywhere
 */
void modulators_step(modulators_t* m, const world_t* w, const body_t* b, float dt_ms)
{
    float dt_s = dt_ms * 0.001f;
    float food = 0.0f;

    world_sample(w, b->x[0], b->y[0], NULL, NULL, &food, NULL);
    m->ser_t = food > 0.05f ? 1.0f : 0.0f;
    m->oct_t = food > 0.05f ? 0.0f : 1.0f;

    /* dopamine reports mechanical contact, drives habituation of the tap path */
    if (b->nose_contact || b->tail_contact)
        m->mech_trace = 1.0f;
    else
        m->mech_trace -= dt_s / DOP_TAU_S;
    if (m->mech_trace < 0.0f)
        m->mech_trace = 0.0f;
    m->dop_t = m->mech_trace;

    m->ser += (dt_s / SER_TAU_S) * (m->ser_t - m->ser);
    m->oct += (dt_s / OCT_TAU_S) * (m->oct_t - m->oct);
    m->dop += (dt_s / DOP_TAU_S) * (m->dop_t - m->dop);
    if (m->ser < 0.0f) m->ser = 0.0f;
    if (m->oct < 0.0f) m->oct = 0.0f;
    if (m->dop < 0.0f) m->dop = 0.0f;
}
