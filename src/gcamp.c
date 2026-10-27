#include "gcamp.h"
#include "config.h"

void gcamp_init(gcamp_t* g)
{
    g->g = 0.0f;
}

float gcamp_step(gcamp_t* g, float act, float dt)
{
    /* asymmetric first order: the binding on step is faster than the
       unbinding decay, chen 2013 gcamp6f kinetics */
    float tau = act > g->g ? GCAMP_TAU_RISE_MS : GCAMP_TAU_DECAY_MS;
    g->g += (dt / tau) * (act - g->g);
    if (g->g < 0.0f) g->g = 0.0f;
    if (g->g > 1.0f) g->g = 1.0f;
    return g->g;
}
