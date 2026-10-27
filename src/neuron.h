#ifndef CE_NEURON_H
#define CE_NEURON_H

#include <stdint.h>
#include "rng.h"

#define MAX_NEURONS     640   /* herm 302 plus muscles, male 579 nodes, headroom */
#define NEURON_NAME_MAX 12    /* fits body wall muscle names like bwm-dl01 */
#define MAX_DELAY_STEPS 64    /* 64 ms ring of delayed presynaptic activation */

typedef enum {
    NT_SENSORY = 0,
    NT_INTER,
    NT_MOTOR,
    NT_COMMAND,   /* ava, avb, avd command pool */
    NT_NTYPES
} neuron_type_t;

typedef struct {
    char  name[NEURON_NAME_MAX];
    uint8_t type;
    uint8_t ablated;     /* pinned to v_rest, functionally silent */
    float v;             /* membrane potential, mv */
    float v_rest;        /* mv */
    float tau_base;      /* membrane tau before modulation, ms */
    float i_chem;        /* chemical synaptic current, mv/ms */
    float i_gap;         /* gap junction current, mv/ms */
    float i_prop;        /* proprioceptive current, mv/ms */
    float i_sens;        /* sensory receptor current, mv/ms */
    float i_bias;        /* standing endogenous current, mv */
    float act_avg;       /* slow running mean of activation, homeostasis */
    float homeo;
    float noise_scale;   /* per neuron membrane noise multiplier */
    float i_adapt;       /* slow adaptation current, mv, hyperpolarizing */
    float hetero;        /* multiplier on outgoing synapse weights, aversive learning */
} neuron_t;

typedef struct {
    int n;
    neuron_t cells[MAX_NEURONS];
    /* ring buffer of activation history for conduction delays */
    float act_hist[MAX_DELAY_STEPS][MAX_NEURONS];
    int hist_head;
} nervous_t;

void  nervous_init(nervous_t* ns, rng_t* rng);
void  nervous_step(nervous_t* ns, float dt, float tau_mod, rng_t* rng);
float neuron_act(const neuron_t* c);   /* transfer to transmitter release 0..1 */
int   nervous_find(const nervous_t* ns, const char* name);
int   nervous_find_prefix(const nervous_t* ns, const char* prefix);
void  nervous_push_history(nervous_t* ns);
float nervous_hist(const nervous_t* ns, int idx, int delay_steps);

#endif
