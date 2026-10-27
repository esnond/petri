#ifndef CE_CONNECTOME_H
#define CE_CONNECTOME_H

#include "neuron.h"

#define MAX_SYN 32768

typedef enum {
    SYN_EXC = 0,   /* chemical, depolarizing */
    SYN_INH,       /* chemical, hyperpolarizing */
    SYN_GAP        /* electrical junction */
} syn_kind_t;

typedef enum {
    STSP_NONE = 0,
    STSP_DEPRESSING,   /* mechanosensory habituation */
    STSP_FACILITATING  /* interneuron sensitization */
} stsp_mode_t;

/*
 * directed graph in compressed sparse row, postsynaptic view.
 * row_ptr[i] .. row_ptr[i+1] lists outgoing synapses of neuron i.
 */
typedef struct {
    int   n_neurons;
    int   n_syn;
    char  names[MAX_NEURONS][NEURON_NAME_MAX];
    int   row_ptr[MAX_NEURONS + 1];
    int   col_ind[MAX_SYN];
    float w[MAX_SYN];        /* anatomical weight, varshney units */
    float w_mult[MAX_SYN];   /* genome scale factor, eval mode */
    uint8_t kind[MAX_SYN];
    uint8_t delay[MAX_SYN];  /* conduction delay, steps of dt */
    uint8_t stsp_mode[MAX_SYN];
    float stsp_x[MAX_SYN];   /* vesicle resource 0..1 */
    float stsp_u[MAX_SYN];   /* utilization 0..1 */
} connectome_t;

int  conn_load_builtin(connectome_t* c, nervous_t* ns);
int  conn_load_csv(connectome_t* c, nervous_t* ns, const char* path);
void conn_classify(connectome_t* c, nervous_t* ns);
int  conn_is_inhibitory(const char* name);

#endif
