#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evolution.h"
#include "sim.h"
#include "config.h"

static void genome_randomize(genome_t* g, rng_t* rng)
{
    int i;
    g->w_chem = 0.5f + rng_uniform(rng);
    g->w_gap = 0.5f + rng_uniform(rng);
    g->tau_scale = 0.5f + rng_uniform(rng);
    for (i = 0; i < MAX_NEURONS; i++) {
        g->tau_mult[i] = 0.8f + 0.4f * rng_uniform(rng);
        g->w_out_mult[i] = 0.8f + 0.4f * rng_uniform(rng);
    }
}

void genome_write_json(const genome_t* g, FILE* out)
{
    int i, first = 1;
    fprintf(out, "{\n  \"w_chem\": %.6f,\n  \"w_gap\": %.6f,\n  \"tau_scale\": %.6f,\n  \"neuron\": {\n", g->w_chem, g->w_gap, g->tau_scale);
    for (i = 0; i < MAX_NEURONS; i++) {
        if (g->tau_mult[i] != 1.0f || g->w_out_mult[i] != 1.0f) {
            if (!first) fprintf(out, ",\n");
            first = 0;
            fprintf(out, "    \"N%d\": {\"tau\": %.6f, \"w_out\": %.6f}", i, g->tau_mult[i], g->w_out_mult[i]);
        }
    }
    fprintf(out, "\n  }\n}\n");
}

static double json_get(const char* js, const char* key, double dflt)
{
    char pat[64];
    const char* p;
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    p = strstr(js, pat);
    if (!p) return dflt;
    p = strchr(p + strlen(pat), ':');
    if (!p) return dflt;
    return strtod(p + 1, NULL);
}

static const char* json_find_neuron(const char* js, const char* name, char* out, size_t outsz)
{
    char pat[32];
    const char* p;
    snprintf(pat, sizeof(pat), "\"%s\"", name);
    p = strstr(js, pat);
    if (!p) return NULL;
    p = strchr(p, '{');
    if (!p) return NULL;
    const char* end = strchr(p, '}');
    if (!end) return NULL;
    size_t len = (size_t)(end - p + 1);
    if (len >= outsz) len = outsz - 1;
    memcpy(out, p, len);
    out[len] = '\0';
    return out;
}

int genome_read_json(genome_t* g, const char* path)
{
    FILE* f = fopen(path, "r");
    long sz;
    char* buf;
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 65536) { fclose(f); return -1; }
    buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return -1; }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return -1;
    }
    buf[sz] = '\0';
    fclose(f);

    g->w_chem = (float)json_get(buf, "w_chem", 1.0);
    g->w_gap = (float)json_get(buf, "w_gap", 1.0);
    g->tau_scale = (float)json_get(buf, "tau_scale", 1.0);

    for (int i = 0; i < MAX_NEURONS; i++) {
        char name[16];
        snprintf(name, sizeof(name), "N%d", i);
        char neuron_json[512];
        if (json_find_neuron(buf, name, neuron_json, sizeof(neuron_json))) {
            g->tau_mult[i] = (float)json_get(neuron_json, "tau", 1.0);
            g->w_out_mult[i] = (float)json_get(neuron_json, "w_out", 1.0);
        } else {
            g->tau_mult[i] = 1.0f;
            g->w_out_mult[i] = 1.0f;
        }
    }
    free(buf);
    return 0;
}

void genome_mutate(genome_t* g, rng_t* rng, float rate, float scale)
{
    int i;
    if (rng_uniform(rng) < rate) g->w_chem *= 1.0f + (rng_uniform(rng) - 0.5f) * 2.0f * scale;
    if (rng_uniform(rng) < rate) g->w_gap *= 1.0f + (rng_uniform(rng) - 0.5f) * 2.0f * scale;
    if (rng_uniform(rng) < rate) g->tau_scale *= 1.0f + (rng_uniform(rng) - 0.5f) * 2.0f * scale;
    for (i = 0; i < MAX_NEURONS; i++) {
        if (rng_uniform(rng) < rate) g->tau_mult[i] *= 1.0f + (rng_uniform(rng) - 0.5f) * 2.0f * scale;
        if (rng_uniform(rng) < rate) g->w_out_mult[i] *= 1.0f + (rng_uniform(rng) - 0.5f) * 2.0f * scale;
    }
    /* clamp */
    if (g->w_chem < 0.01f) g->w_chem = 0.01f;
    if (g->w_chem > 10.0f) g->w_chem = 10.0f;
    if (g->w_gap < 0.01f) g->w_gap = 0.01f;
    if (g->w_gap > 10.0f) g->w_gap = 10.0f;
    if (g->tau_scale < 0.1f) g->tau_scale = 0.1f;
    if (g->tau_scale > 10.0f) g->tau_scale = 10.0f;
    for (i = 0; i < MAX_NEURONS; i++) {
        if (g->tau_mult[i] < 0.1f) g->tau_mult[i] = 0.1f;
        if (g->tau_mult[i] > 10.0f) g->tau_mult[i] = 10.0f;
        if (g->w_out_mult[i] < 0.01f) g->w_out_mult[i] = 0.01f;
        if (g->w_out_mult[i] > 10.0f) g->w_out_mult[i] = 10.0f;
    }
}

void genome_crossover(const genome_t* a, const genome_t* b, genome_t* child, rng_t* rng)
{
    int i;
    float alpha = rng_uniform(rng);
    child->w_chem = a->w_chem * alpha + b->w_chem * (1.0f - alpha);
    child->w_gap = a->w_gap * alpha + b->w_gap * (1.0f - alpha);
    child->tau_scale = a->tau_scale * alpha + b->tau_scale * (1.0f - alpha);
    for (i = 0; i < MAX_NEURONS; i++) {
        float a2 = rng_uniform(rng);
        child->tau_mult[i] = a->tau_mult[i] * a2 + b->tau_mult[i] * (1.0f - a2);
        float a3 = rng_uniform(rng);
        child->w_out_mult[i] = a->w_out_mult[i] * a3 + b->w_out_mult[i] * (1.0f - a3);
    }
}

static void apply_genome_to_sim(sim_t* s, const genome_t* g)
{
    int i, j;
    s->g_chem_scale = g->w_chem;
    s->g_gap_scale = g->w_gap;
    s->tau_scale_global = g->tau_scale;
    for (i = 0; i < s->ns.n; i++) {
        s->ns.cells[i].tau_base *= g->tau_mult[i];
        for (j = s->conn.row_ptr[i]; j < s->conn.row_ptr[i + 1]; j++)
            s->conn.w_mult[j] *= g->w_out_mult[i];
    }
}

/* the head starts at body_init coordinates, keep these two in sync with it */
#define EVAL_START_X_MM 6.0f
#define EVAL_START_Y_MM 8.0f

/* one megabyte of sim state, static keeps it off the stack */
static sim_t eval_sim;

static int g_cost_mode = 0;

static double evaluate_genome_impl(const genome_t* g, int duration_s,
                                   const char* connectome_path, uint64_t seed,
                                   int fitness_net, double* other_out)
{
    g_cost_mode = 0; /* cleared, cost mode sets it before the call */
    sim_opts_t opts;
    double path, net;
    memset(&opts, 0, sizeof(opts));
    opts.duration_s = duration_s;
    opts.seed = seed;
    opts.connectome_path = connectome_path;
    opts.headless = 1;
    opts.quiet = 1;

    if (sim_init(&eval_sim, &opts) != 0) return -1.0;

    apply_genome_to_sim(&eval_sim, g);
    sim_run(&eval_sim, NULL, NULL);

    /* both metrics are always computed, the log shows each one so the
       difference between crawling and undulating in place stays visible */
    path = (double)eval_sim.metrics.odometer_mm;
    {
        float dx = eval_sim.body.x[0] - EVAL_START_X_MM;
        float dy = eval_sim.body.y[0] - EVAL_START_Y_MM;
        net = hypotf(dx, dy);
    }
    if (g_cost_mode) {
        double cost = (double)eval_sim.metrics.energy_cost + 1.0;
        double spd_per_cost = net / cost;
        if (other_out)
            *other_out = (double)eval_sim.metrics.energy_cost;
        return spd_per_cost;
    }
    if (other_out)
        *other_out = (fitness_net ? path : net);
    return fitness_net ? net : path;
}

double evaluate_genome(genome_t* g, int duration_s, const char* connectome_path,
                       uint64_t seed, int fitness_net, double* other_out)
{
    return evaluate_genome_impl(g, duration_s, connectome_path, seed,
                                fitness_net, other_out);
}


/* ---- inverse problem: evolve a genome whose head visits a drawn path ---- */
static int cmp_fitness_desc(const void* a, const void* b);
#define INV_MAX_WP 64
#define INV_TRAJ 600                 /* head samples, 0.5 s apart */
static float inv_wpx[INV_MAX_WP], inv_wpy[INV_MAX_WP];
static int inv_nwp = 0;
static float inv_trx[INV_TRAJ], inv_try[INV_TRAJ];

int evolution_load_path(const char* path)
{
    /* json [[x,y],[x,y],...] in dish millimeters */
    FILE* f = fopen(path, "r");
    char buf[8192];
    const char* p;
    size_t n;
    if (!f) return -1;
    n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';
    fclose(f);
    /* the file holds only numbers, commas and brackets, so a plain
       strtod walk collects the coordinate pairs without json nesting
       puzzles */
    inv_nwp = 0;
    p = buf;
    while (inv_nwp + 1 < INV_MAX_WP && *p != '\0') {
        char* end;
        double x = strtod(p, &end);
        if (end == p) {
            if (*p == '\0')
                break;
            p++;
            continue;
        }
        {
            const char* q = end;
            double y;
            while (*q && (*q < '0' || *q > '9') && *q != '-')
                q++;
            if (!*q)
                break;
            y = strtod(q, &end);
            inv_wpx[inv_nwp] = (float)x;
            inv_wpy[inv_nwp] = (float)y;
            inv_nwp++;
            p = end;
        }
    }
    fprintf(stderr, "inverse: parsed %d waypoints, first (%.1f, %.1f)\n", inv_nwp, inv_wpx[0], inv_wpy[0]);
    return inv_nwp;
}

static double evaluate_inverse(const genome_t* g, int duration_s,
                               const char* connectome_path, uint64_t seed)
{
    sim_opts_t opts;
    long n_steps, i, cap = 0;
    double err = 0.0;
    int k;
    memset(&opts, 0, sizeof(opts));
    opts.duration_s = duration_s;
    opts.seed = seed;
    opts.connectome_path = connectome_path;
    opts.headless = 1;
    opts.quiet = 1;
    if (sim_init(&eval_sim, &opts) != 0) return 1e9;
    apply_genome_to_sim(&eval_sim, g);
    n_steps = (long)(duration_s * 1000.0 / DT_MS);
    for (i = 0; i < n_steps; i++) {
        sim_step(&eval_sim);
        if (i % 500 == 0 && cap < INV_TRAJ) {
            inv_trx[cap] = eval_sim.body.x[0];
            inv_try[cap] = eval_sim.body.y[0];
            cap++;
        }
    }
    /* every waypoint must be visited at some time: mean of per waypoint
       nearest approaches, in body lengths */
    for (k = 0; k < inv_nwp; k++) {
        float best = 1e9f;
        for (i = 0; i < cap; i++) {
            float dx = inv_trx[i] - inv_wpx[k];
            float dy = inv_try[i] - inv_wpy[k];
            float d = sqrtf(dx * dx + dy * dy);
            if (d < best) best = d;
        }
        err += best;
    }
    if (inv_nwp) err /= inv_nwp;
    return -err;
}

int evolution_run_inverse(const char* path_json, int generations, int eval_duration_s,
                          const char* connectome_path, const char* output_path)
{
    evolution_t e;
    int gen, i;
    genome_t child;
    char nm[128];
    if (evolution_load_path(path_json) < 2) {
        fprintf(stderr, "inverse: need at least two waypoints in %s\n", path_json);
        return -1;
    }
    evolution_init(&e, 1, 16, 0);
    for (gen = 0; gen < generations; gen++) {
        uint64_t gen_seed = rng_next(&e.rng);
        double best = -1e18;
        for (i = 0; i < e.pop_size; i++) {
            double fit = evaluate_inverse(&e.population[i].genome, eval_duration_s,
                                          connectome_path, gen_seed);
            e.population[i].fitness = fit;
            if (fit > best) best = fit;
        }
        qsort(e.population, e.pop_size, sizeof(individual_t), cmp_fitness_desc);
        fprintf(stderr, "inverse gen %d: best err %.3f body lengths\n", gen,
                -e.population[0].fitness);
        if (gen % 5 == 0 || gen == generations - 1) {
            /* trajectory of the current best, for the convergence figure */
            double keep = evaluate_inverse(&e.population[0].genome, eval_duration_s,
                                           connectome_path, gen_seed);
            (void)keep;
            snprintf(nm, sizeof(nm), "%s_gen%02d.csv", output_path, gen);
            FILE* f = fopen(nm, "w");
            if (f) {
                fprintf(f, "x_mm,y_mm\n");
                for (i = 0; i < INV_TRAJ; i++)
                    fprintf(f, "%.3f,%.3f\n", inv_trx[i], inv_try[i]);
                fclose(f);
            }
        }
        for (i = e.pop_size / 2; i < e.pop_size; i++) {
            int p1 = rng_next(&e.rng) % (e.pop_size / 2);
            int p2 = rng_next(&e.rng) % (e.pop_size / 2);
            genome_crossover(&e.population[p1].genome, &e.population[p2].genome,
                             &child, &e.rng);
            genome_mutate(&child, &e.rng, e.mutation_rate, e.mutation_scale);
            e.population[i].genome = child;
            e.population[i].evaluated = 0;
        }
        {
            FILE* gf = fopen(output_path, "w");
            if (gf) {
                genome_write_json(&e.population[0].genome, gf);
                fclose(gf);
            }
        }
    }
    return 0;
}

void evolution_init(evolution_t* e, uint64_t seed, int pop_size, int fitness_net)
{
    /* fitness modes: 0 path, 1 net, 2 speed per unit muscle work */
    e->fitness_cost = (fitness_net == 2);
    fitness_net = (fitness_net == 1);
    if (pop_size > 64) pop_size = 64;
    if (pop_size < 4) pop_size = 4;
    e->pop_size = pop_size;
    e->generation = 0;
    e->n_elite = pop_size / 10;
    e->fitness_net = fitness_net ? 1 : 0;
    if (e->n_elite < 1) e->n_elite = 1;
    e->mutation_rate = 0.15f;
    e->mutation_scale = 0.2f;
    rng_seed(&e->rng, seed);
    for (int i = 0; i < pop_size; i++) {
        genome_randomize(&e->population[i].genome, &e->rng);
        e->population[i].fitness = -1.0;
        e->population[i].evaluated = 0;
    }
}

static int cmp_fitness_desc(const void* a, const void* b)
{
    double fa = ((const individual_t*)a)->fitness;
    double fb = ((const individual_t*)b)->fitness;
    return (fa > fb) ? -1 : (fa < fb) ? 1 : 0;
}

int evolution_run(evolution_t* e, int generations, int eval_duration_s,
                  const char* connectome_path, const char* output_path)
{
    int gen, i;
    genome_t child_genome;

    for (gen = 0; gen < generations; gen++) {
        int n_parents;
        e->generation = gen;
        fprintf(stderr, "=== Generation %d ===\n", gen);
        /* one seed per generation, all individuals scored on the same noise
           realization so their fitness values are comparable */
        {
            uint64_t gen_seed = rng_next(&e->rng);
            for (i = 0; i < e->pop_size; i++) {
                if (!e->population[i].evaluated) {
                    double other = 0.0;
                    double fit;
                    g_cost_mode = e->fitness_cost;
                    fit = evaluate_genome(&e->population[i].genome, eval_duration_s,
                                          connectome_path, gen_seed,
                                          e->fitness_net, &other);
                    g_cost_mode = 0;
                    e->population[i].fitness = fit;
                    e->population[i].evaluated = 1;
                    fprintf(stderr, "  ind %d: fitness = %.2f %s = %.2f mm\n", i, fit,
                            e->fitness_net ? "path" : "net", other);
                }
            }
        }

        /* sort by fitness descending */
        qsort(e->population, e->pop_size, sizeof(individual_t), cmp_fitness_desc);

        fprintf(stderr, "  best: %.2f, avg: ", e->population[0].fitness);
        double sum = 0;
        for (i = 0; i < e->pop_size; i++) sum += e->population[i].fitness;
        fprintf(stderr, "%.2f\n", sum / e->pop_size);

        /* save best genome */
        if (output_path) {
            FILE* f = fopen(output_path, "w");
            if (f) {
                genome_write_json(&e->population[0].genome, f);
                fclose(f);
            } else {
                fprintf(stderr, "evolution: cannot write %s\n", output_path);
            }
        }

        /* create next generation */
        if (gen + 1 < generations) {
            individual_t new_pop[64];
            int idx = 0;

            /* elitism, copy the best individuals untouched */
            for (i = 0; i < e->n_elite; i++) {
                new_pop[idx++] = e->population[i];
            }

            /* parents from the top half, elite only selection collapses
               diversity to a handful of genomes within a few generations */
            n_parents = e->pop_size / 2;
            if (n_parents < 2)
                n_parents = 2;
            while (idx < e->pop_size) {
                int p1 = (int)(rng_uniform(&e->rng) * n_parents);
                int p2 = (int)(rng_uniform(&e->rng) * n_parents);
                if (p1 >= n_parents) p1 = n_parents - 1;
                if (p2 >= n_parents) p2 = n_parents - 1;
                genome_crossover(&e->population[p1].genome, &e->population[p2].genome,
                                 &child_genome, &e->rng);
                genome_mutate(&child_genome, &e->rng, e->mutation_rate, e->mutation_scale);
                new_pop[idx].genome = child_genome;
                new_pop[idx].fitness = -1.0;
                new_pop[idx].evaluated = 0;
                idx++;
            }

            /* replace population */
            for (i = 0; i < e->pop_size; i++) {
                e->population[i] = new_pop[i];
            }
        }
    }
    return 0;
}