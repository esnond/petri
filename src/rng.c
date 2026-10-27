#include <math.h>
#include "rng.h"

void rng_seed(rng_t* r, uint64_t seed)
{
    /* zero state is a fixed point of xorshift, fold seed bits to avoid it */
    r->s = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    if (r->s == 0)
        r->s = 88172645463325252ULL;
    r->normal_have = 0;
}

uint64_t rng_next(rng_t* r)
{
    uint64_t x = r->s;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    r->s = x;
    return x * 2685821657736338717ULL;
}

float rng_uniform(rng_t* r)
{
    /* 24 bits of mantissa, exact in float */
    return (float)(rng_next(r) >> 40) * (1.0f / 16777216.0f);
}

float rng_normal(rng_t* r)
{
    /* box muller, second sample cached in the generator itself, a static
       here would leak a sample across reseeds and break reproducibility
       of any run that restarts the stream */
    float u1, u2, amp;

    if (r->normal_have) {
        r->normal_have = 0;
        return r->normal_cache;
    }
    do {
        u1 = rng_uniform(r);
    } while (u1 <= 1e-7f);
    u2 = rng_uniform(r) * 6.283185307f;
    amp = sqrtf(-2.0f * logf(u1));
    r->normal_cache = amp * sinf(u2);
    r->normal_have = 1;
    return amp * cosf(u2);
}
