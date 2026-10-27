/* named mutants, phenotypic levers only: excitability, synaptic
   release, gaba, gap junctions, muscle drive. brenner 1974 catalogues
   the loci; the levers here are the model equivalents, each marked. */
#include <string.h>
#include <stdio.h>
#include "mutants.h"
#include "sim.h"
#include "config.h"

static int mn_like(const char* n)
{
    return strncmp(n, "DB", 2) == 0 || strncmp(n, "VB", 2) == 0 ||
           strncmp(n, "DA", 2) == 0 || strncmp(n, "VA", 2) == 0 ||
           strncmp(n, "DD", 2) == 0 || strncmp(n, "VD", 2) == 0;
}
void apply_mutant(sim_t* s, const char* name)
{
    nervous_t* ns = &s->ns;
    connectome_t* c = &s->conn;
    int i, j;
    if (strcmp(name, "unc-8") == 0) {
        /* degenerate stretch receptor channel: command neurons hyperexcitable,
           brenner 1974 locomotion defects */
        for (i = 0; i < ns->n; i++)
            if (strncmp(ns->cells[i].name, "AV", 2) == 0)
                ns->cells[i].tau_base *= 0.7f;
    } else if (strcmp(name, "unc-9") == 0) {
        /* innexin, gap junction loss between command neurons,
         brenner 1974; shunt the command pair gaps */
        for (i = 0; i < ns->n; i++) {
            if (strncmp(ns->cells[i].name, "AV", 2) != 0 &&
                strncmp(ns->cells[i].name, "AIB", 3) != 0)
                continue;
            for (j = c->row_ptr[i]; j < c->row_ptr[i + 1]; j++)
                if (c->kind[j] == 2) c->w_mult[j] = 0.0f;
        }
    } else if (strcmp(name, "unc-54") == 0) {
        /* myosin heavy chain, body wall muscle full of gaps,
         epstein and waterston 1974; half the neuromuscular drive */
        s->nmj_gain_scale = 0.5f;
    } else if (strcmp(name, "unc-54-severe") == 0) {
        s->nmj_gain_scale = 0.0f;
    } else if (strcmp(name, "unc-22") == 0) {
        /* twitchin, twitching body wall, waterston 1980; triple the
           motor neuron noise as the twitch source */
        for (i = 0; i < ns->n; i++)
            if (mn_like(ns->cells[i].name))
                ns->cells[i].noise_scale = 3.0f;
    } else if (strcmp(name, "unc-29") == 0 || strcmp(name, "unc-38") == 0 ||
               strcmp(name, "unc-63") == 0) {
        /* acetylcholine receptor subunits, levamisole resistant,
         lewis 1980; half the cholinergic neuromuscular weight */
        for (i = 0; i < ns->n; i++)
            if (mn_like(ns->cells[i].name))
                for (j = c->row_ptr[i]; j < c->row_ptr[i + 1]; j++)
                    if (c->kind[j] != 2) c->w_mult[j] *= 0.5f;
    } else if (strcmp(name, "unc-13") == 0 || strcmp(name, "unc-18") == 0 ||
               strcmp(name, "unc-64") == 0 || strcmp(name, "unc-10") == 0) {
        /* exocytosis and vesicle priming, richmond 1999; release
           probability halved through the stsp utilization */
        s->stsp_u_scale = 0.5f;
    } else if (strcmp(name, "unc-31") == 0) {
        /* CAPS homolog, dense core vesicle secretion, speese 1998;
           freeze the neuromodulator state at baseline */
        s->mod_frozen = 1.0f;
    } else if (strcmp(name, "unc-25") == 0) {
        /* glutamic acid decarboxylase, no gaba, jin 1999; the
           inhibitory d class outputs go silent */
        for (i = 0; i < ns->n; i++)
            if (strncmp(ns->cells[i].name, "DD", 2) == 0 ||
                strncmp(ns->cells[i].name, "VD", 2) == 0)
                for (j = c->row_ptr[i]; j < c->row_ptr[i + 1]; j++)
                    c->w_mult[j] = 0.0f;
    } else if (strcmp(name, "unc-49") == 0) {
        /* gaba-a receptor, muscles deaf to inhibition, bamber 1999;
           same phenotypic lever as unc-25 from the receiver side */
        for (i = 0; i < ns->n; i++)
            if (strncmp(ns->cells[i].name, "DD", 2) == 0 ||
                strncmp(ns->cells[i].name, "VD", 2) == 0)
                for (j = c->row_ptr[i]; j < c->row_ptr[i + 1]; j++)
                    c->w_mult[j] = 0.0f;
    } else if (strcmp(name, "unc-77") == 0) {
        /* sodium channel family, slower membranes */
        for (i = 0; i < ns->n; i++)
            ns->cells[i].tau_base *= 1.3f;
    } else if (strcmp(name, "unc-58") == 0) {
        /* potassium conductance, faster repolarization */
        for (i = 0; i < ns->n; i++)
            ns->cells[i].tau_base *= 0.7f;
    } else if (strcmp(name, "unc-103") == 0) {
        /* erg like k channel, posterior bias, ohnishi 2001 */
        for (i = 0; i < ns->n; i++)
            if (ns->cells[i].name[0] == 'D' || ns->cells[i].name[0] == 'V')
                ns->cells[i].tau_base *= 1.2f;
    } else {
        fprintf(stderr, "mutant: unknown %s\n", name);
    }
}
