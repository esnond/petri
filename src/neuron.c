#include <math.h>
#include <string.h>
#include "neuron.h"
#include "config.h"

/* graded transmitter release, no spikes anywhere in this model */
float neuron_act(const neuron_t* c)
{
    return 1.0f / (1.0f + expf(-(c->v - ACT_HALF_MV) / ACT_SLOPE_MV));
}

int nervous_find(const nervous_t* ns, const char* name)
{
    int i;
    for (i = 0; i < ns->n; i++)
        if (strcmp(ns->cells[i].name, name) == 0)
            return i;
    return -1;
}

int nervous_find_prefix(const nervous_t* ns, const char* prefix)
{
    size_t len = strlen(prefix);
    int i;
    for (i = 0; i < ns->n; i++)
        if (strncmp(ns->cells[i].name, prefix, len) == 0)
            return i;
    return -1;
}

void nervous_init(nervous_t* ns, rng_t* rng)
{
    int i;
    ns->n = 0;
    ns->hist_head = 0;
    memset(ns->act_hist, 0, sizeof(ns->act_hist));
    (void)rng;
    /* no rng here: the pool is capacity sized and the per step noise must
       land on the same stream positions no matter how much headroom the
       pool has, otherwise changing max neurons would silently reseed the
       whole run */
    for (i = 0; i < MAX_NEURONS; i++) {
        neuron_t* c = &ns->cells[i];
        memset(c, 0, sizeof(*c));
        c->v = V_REST_MV;
        c->v_rest = V_REST_MV;
        c->tau_base = 25.0f;
        c->homeo = 1.0f;
        c->noise_scale = 1.0f;
        c->i_adapt = 0.0f;
        c->hetero = 1.0f;
        c->act_avg = 0.2f;
    }
}

void nervous_push_history(nervous_t* ns)
{
    int i;
    ns->hist_head = (ns->hist_head + 1) % MAX_DELAY_STEPS;
    for (i = 0; i < ns->n; i++) {
        /* an ablated cell must not keep transmitting: its membrane is
           clamped to rest which still maps to a nonzero act, and the
           delayed history would drive its postsynaptic partners with a
           ghost tone */
        ns->act_hist[ns->hist_head][i] =
            ns->cells[i].ablated ? 0.0f : neuron_act(&ns->cells[i]);
    }
}

float nervous_hist(const nervous_t* ns, int idx, int delay_steps)
{
    int t = ns->hist_head - delay_steps;
    while (t < 0)
        t += MAX_DELAY_STEPS;
    return ns->act_hist[t][idx];
}

/*
 * semi implicit euler on the leak term keeps the fixed point exact for any
 * dt, the synaptic currents are one step explicit which is stable for the
 * conductance range allowed by config.h
 */
void nervous_step(nervous_t* ns, float dt, float tau_mod, rng_t* rng)
{
    int i;
    for (i = 0; i < ns->n; i++) {
        neuron_t* c = &ns->cells[i];
        /* spike frequency adaptation, command pool only: kato 2015
           shows the ava transient rises and decays within a reversal.
           the cell hyperpolarizes with its own activity and recovers
           on its own clock, which makes a command burst transient
           instead of a held plateau. motor neurons keep their graded
           tone, the wave needs it */
        if (c->type == NT_COMMAND)
            c->i_adapt += dt / ADAPT_TAU_MS * (neuron_act(c) * ADAPT_GAIN_MV - c->i_adapt);
        float noise = NOISE_SD_MV * c->noise_scale * sqrtf(dt);
        float tau, i_tot;
        if (c->ablated) {
            c->v = c->v_rest;
            c->i_chem = c->i_gap = c->i_prop = c->i_sens = 0.0f;
            continue;
        }
        tau = c->tau_base * tau_mod;
        if (tau < TAU_MIN_MS)
            tau = TAU_MIN_MS;
        i_tot = c->i_chem + c->i_gap + c->i_prop + c->i_sens + c->i_bias;
        c->v += (dt / tau) * (c->v_rest - c->v) + (dt / tau) * i_tot
                + noise * rng_normal(rng);
        /* graded potentials stay in a physiological band */
        if (c->v > 0.0f)
            c->v = 0.0f;
        if (c->v < -90.0f)
            c->v = -90.0f;
    }
}
