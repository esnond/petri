#ifndef CE_RNG_H
#define CE_RNG_H

#include <stdint.h>

/* xorshift64*, deterministic, no libc rand */
typedef struct {
    uint64_t s;
    float    normal_cache;   /* box muller second sample, state not global */
    int      normal_have;
} rng_t;

void   rng_seed(rng_t* r, uint64_t seed);
uint64_t rng_next(rng_t* r);
float  rng_uniform(rng_t* r);   /* [0,1) */
float  rng_normal(rng_t* r);    /* unit variance, box muller */

#endif
