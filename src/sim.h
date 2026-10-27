#ifndef CE_SIM_H
#define CE_SIM_H

#include <stdint.h>
#include "config.h"
#include "connectome.h"
#include "body.h"
#include "world.h"
#include "modulators.h"
#include "metrics.h"

#define MAX_ABLATE 32

typedef struct {
    double duration_s;
    uint64_t seed;
    const char* connectome_path;
    const char* eval_path;
    const char* ablate_list;
    int headless;
    int quiet;
    int ablate_screen;
    int no_food;
    int thermo;
    float tap_interval_ms;   /* 0 disables automatic taps */
    int evolve_gens;         /* number of generations for --evolve mode */
    int population;          /* evolution population size, 0 means default */
    int evolve_fitness_net;  /* 1 scores net displacement instead of path length */
    int evolve_fitness_cost; /* 1 scores speed per unit muscle work */
    int conditioning;        /* 0 none, 1 paired odor plus ash, 2 odor only control */
    const char* surgery_list;  /* comma separated neurons to cut at init */
    const char* trace_path;    /* gcamp trace dump of the command pool */
    const char* opsin_list;    /* "AVA:ChR2,AVB:NpHR" */
    const char* light_protocol;/* json light events */
    const char* light_zone;    /* "cx,cy,r_mm" closed loop circle */
    const char* mutant;        /* named mutant, see src/mutants.c */
    const char* trace_all_path; /* all neuron membrane potentials, interp export */
    const char* edges_path;    /* ground truth adjacency dump */
    const char* inverse_path;  /* target path json for the inverse problem */
    int swarm_n;               /* >0 runs N worms in one dish */
    int swarm_seed;
    uint64_t shuffle_seed;     /* nonzero rewires targets degree preserving */
    const char* evolve_output; /* output path for best genome */
} sim_opts_t;

#define SURGERY_MAX_OPS 16

#define SURGERY_POOL 8192
#define MAX_OPSINS 8
#define MAX_LIGHT_EVENTS 16
typedef struct {
    char     neuron[NEURON_NAME_MAX];
    int      idx;        /* cell index, -1 unknown */
    int      inhibitory; /* 1 for NpHR style shunt, 0 for cheta */
    float    state;      /* opsin open probability 0..1 */
} opsin_t;
typedef struct {
    float t_start, t_end, intensity;
    char   target[NEURON_NAME_MAX];
} light_event_t;

typedef struct {
    const char* opsin_list;    /* "AVA:ChR2,AVB:NpHR" */
    const char* light_protocol;/* json light events */
    const char* light_zone;    /* "cx,cy,r_mm" closed loop circle */
    const char* mutant;        /* named mutant, see src/mutants.c */
    int    n_opsins;
    opsin_t opsins[MAX_OPSINS];
    int    n_light_events;
    light_event_t light[MAX_LIGHT_EVENTS];
    float  light_zone_rect[3]; /* cx, cy, radius, radius 0 disables */
    float  nmj_gain_scale;     /* mutant knobs, default 1 */
    float  stsp_u_scale;
    float  mod_frozen;
    float  muscle_noise_scale;

    sim_opts_t opts;
    nervous_t  ns;
    connectome_t conn;
    body_t     body;
    world_t    world;
    modulators_t mod;
    metrics_t  metrics;
    rng_t      rng;
    int   idx_ava, idx_avb, idx_aib, idx_aiy, idx_rim;
    int   idx_ash[4]; int n_ash;
    int   idx_awc[4]; int n_awc;
    int   idx_alplm[4]; int n_alplm;
    int   idx_awa[4]; int n_awa;
    int   idx_ase[4]; int n_ase;
    int   idx_afd[4]; int n_afd;
    double t_ms;
    float  tap_timer_ms;
    int    tap_active_ms;
    float  mec_trace;        /* decaying mechanoreceptor activation 0..1 */
    float  ash_trace;        /* adapting nociceptor activation 0..1 */
    int    n_surgery;
    int    edges_dumped;        /* completed cut operations this session */
    int    surgery_cut[MAX_NEURONS];      /* per neuron: 1 when outputs cut */
    float  surgery_pool[SURGERY_POOL];    /* saved weights, fifo per op */
    int    surgery_pool_used;
    int    prev_contact;     /* previous step wall or tap contact, edge detect */
    int    prev_nose_contact; /* previous step nose contact, edge detect */
    float  g_chem_scale;    /* genome knobs, eval mode */
    float  g_gap_scale;
    float  tau_scale_global;
} sim_t;

int    sim_init(sim_t* s, const sim_opts_t* o);
void   sim_reset_dynamics(sim_t* s);
void   sim_step(sim_t* s);
double sim_fitness(const sim_t* s);
int    sim_apply_genome(sim_t* s, const char* path);
int    sim_run(sim_t* s, void (*frame_cb)(sim_t*, void*), void* cb_arg);
int    sim_ablate_screen(sim_t* s, double per_neuron_s);
/* interactive neurosurgery: zero or restore the outgoing chemical and
   gap weights of one neuron, with a json session log for replay */
int    sim_cut_outputs(sim_t* s, const char* name);
int    sim_restore_outputs(sim_t* s, const char* name);
int    sim_surgery_iscut(sim_t* s, int idx);
void   sim_surgery_session(sim_t* s, const char* path);

#endif
