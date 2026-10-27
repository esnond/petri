/* minimal c api for libpetri: open a simulation, step it, read the
   head and the metrics. no main, no terminal, pure library. */
#include <stdint.h>
#include <string.h>
#include "sim.h"

static sim_t g_sim;
static int g_open = 0;

int petri_open(const char* connectome_path, uint64_t seed, double duration_s)
{
    sim_opts_t opts;
    memset(&opts, 0, sizeof(opts));
    opts.duration_s = duration_s;
    opts.seed = seed;
    opts.headless = 1;
    opts.quiet = 1;
    opts.connectome_path = connectome_path;
    if (sim_init(&g_sim, &opts) != 0)
        return -1;
    g_open = 1;
    return 0;
}

int petri_open_default(uint64_t seed, double duration_s)
{
    return petri_open(NULL, seed, duration_s);
}

void petri_steps(long n)
{
    long i;
    if (!g_open)
        return;
    for (i = 0; i < n; i++)
        sim_step(&g_sim);
}

float petri_speed(void)   { return g_open ? g_sim.body.speed : 0.0f; }
float petri_head_x(void)  { return g_open ? g_sim.body.x[0] : 0.0f; }
float petri_head_y(void)  { return g_open ? g_sim.body.y[0] : 0.0f; }
double petri_time(void)   { return g_open ? g_sim.metrics.t_s : 0.0; }
long petri_reversals(void){ return g_open ? g_sim.metrics.n_reversals : 0; }
