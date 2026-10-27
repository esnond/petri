#include <string.h>
#include "plasticity.h"
#include "config.h"

void plasticity_init(connectome_t* c, nervous_t* ns)
{
    int i;
    (void)ns;
    for (i = 0; i < c->n_syn; i++) {
        c->stsp_x[i] = 1.0f;
        c->stsp_u[i] = c->stsp_mode[i] == STSP_FACILITATING ? STSP_U_FAC : STSP_U_DEP;
        c->w_mult[i] = 1.0f;
    }
}

/*
 * tsodyks markram state lives in the csr and is advanced in sim.c while the
 * presynaptic row is being walked. this file keeps the slow synaptic
 * plasticity, homeostatic and heterosynaptic.
 */
void plasticity_step(connectome_t* c, nervous_t* ns, modulators_t* m, float dt)
{
    int i;
    float ash_act = 0.0f, awc_act = 0.0f;

    /* heterosynaptic aversive learning, coinidence of odor pathway activity
       and ash punishment under octopamine hunger weakens attractive synapses */
    for (i = 0; i < ns->n; i++) {
        const char* nm = ns->cells[i].name;
        float act = neuron_act(&ns->cells[i]);
        float avg = ns->cells[i].act_avg;

        /* slow running mean of activation */
        ns->cells[i].act_avg += (dt / HOMEO_TAU_MS) * (act - avg);

        /* homeostatic scaling of outgoing weights toward a target rate */
        {
            float target = HOMEO_TARGET;
            float corr = target / (avg > 0.02f ? avg : 0.02f);
            if (corr < HOMEO_MIN) corr = HOMEO_MIN;
            if (corr > HOMEO_MAX) corr = HOMEO_MAX;
            ns->cells[i].homeo += (dt / HOMEO_TAU_MS) * (corr - ns->cells[i].homeo);
        }

        if (strncmp(nm, "ASH", 3) == 0 && act > ash_act)
            ash_act = act;
        if (strncmp(nm, "AWC", 3) == 0 && act > awc_act)
            awc_act = act;
    }

    if (ash_act > 0.25f && awc_act > 0.25f && m->oct > 0.3f) {
        float elig = HETERO_ETA * dt * ash_act * awc_act * m->oct;
        for (i = 0; i < ns->n; i++) {
            const char* nm = ns->cells[i].name;
            /* suppress the attractive odor pathways, awc and awa to first order interneurons */
            if (strncmp(nm, "AWC", 3) == 0 || strncmp(nm, "AWA", 3) == 0) {
                ns->cells[i].hetero -= elig;
                if (ns->cells[i].hetero < HETERO_MIN)
                    ns->cells[i].hetero = HETERO_MIN;
            }
        }
    } else {
        /* slow spontaneous recovery of suppressed pathways */
        for (i = 0; i < ns->n; i++) {
            if (ns->cells[i].hetero < 1.0f) {
                ns->cells[i].hetero += dt / HETERO_TAU_MS;
                if (ns->cells[i].hetero > 1.0f)
                    ns->cells[i].hetero = 1.0f;
            }
        }
    }
    (void)c;
}
