#ifndef CE_EVOLUTION_H
#define CE_EVOLUTION_H

#include <stdio.h>
#include "sim.h"
#include "rng.h"

typedef struct {
    float w_chem;
    float w_gap;
    float tau_scale;
    float tau_mult[MAX_NEURONS];
    float w_out_mult[MAX_NEURONS];
} genome_t;

typedef struct {
    genome_t genome;
    double fitness;
    int evaluated;
} individual_t;

typedef struct {
    individual_t population[64];
    int pop_size;
    int generation;
    int n_elite;
    int fitness_net;         /* 1 scores net displacement, 0 scores path length */
    int fitness_cost;        /* 1 scores speed per unit muscle work */
    float mutation_rate;
    float mutation_scale;
    rng_t rng;
} evolution_t;

void evolution_init(evolution_t* e, uint64_t seed, int pop_size, int fitness_mode);
int    evolution_load_path(const char* path);
int    evolution_run_inverse(const char* path_json, int generations,
                             int eval_duration_s, const char* connectome_path,
                             const char* output_path);
int evolution_run(evolution_t* e, int generations, int eval_duration_s,
                  const char* connectome_path, const char* output_path);
void genome_write_json(const genome_t* g, FILE* out);
int genome_read_json(genome_t* g, const char* path);
void genome_mutate(genome_t* g, rng_t* rng, float rate, float scale);
void genome_crossover(const genome_t* a, const genome_t* b, genome_t* child, rng_t* rng);
double evaluate_genome(genome_t* g, int duration_s, const char* connectome_path,
                       uint64_t seed, int fitness_net, double* other_out);

#endif