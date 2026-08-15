/* simd_f128_random.h
 *
 * high-entropy random number generation for 128-bit floating point operations.
 * implements xoshiro256++ to produce perfect 106-bit uniform distributions.
 *
 * updated 2026-08-13
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#ifndef SIMD_F128_RANDOM_H
#define SIMD_F128_RANDOM_H

#include <stdint.h>

#include "simd_f128.h"

#ifdef __cplusplus
extern "C" {
#endif

// ██████   █████  ███    ██ ██████   ██████  ███    ███
// ██   ██ ██   ██ ████   ██ ██   ██ ██    ██ ████  ████
// ██████  ███████ ██ ██  ██ ██   ██ ██    ██ ██ ████ ██
// ██   ██ ██   ██ ██  ██ ██ ██   ██ ██    ██ ██  ██  ██
// ██   ██ ██   ██ ██   ████ ██████   ██████  ██      ██
//
// >>random number generation api
// xoshiro256++ state
typedef struct {
    uint64_t s[4];
} simd_f128_prng;

static inline uint64_t _simd_f128_rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

// generate next 64-bit random number
static inline uint64_t simd_f128_prng_next(simd_f128_prng* state) {
    const uint64_t result = _simd_f128_rotl(state->s[0] + state->s[3], 23) + state->s[0];
    const uint64_t t = state->s[1] << 17;
    state->s[2] ^= state->s[0];
    state->s[3] ^= state->s[1];
    state->s[1] ^= state->s[2];
    state->s[0] ^= state->s[3];
    state->s[2] ^= t;
    state->s[3] = _simd_f128_rotl(state->s[3], 45);
    return result;
}

// splitmix64 for seeding
static inline uint64_t _simd_f128_splitmix64(uint64_t* state) {
    uint64_t z = (*state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

// seed the prng
SIMD_F128_INLINE void simd_f128_prng_seed(simd_f128_prng* state, uint64_t seed) {
    state->s[0] = _simd_f128_splitmix64(&seed);
    state->s[1] = _simd_f128_splitmix64(&seed);
    state->s[2] = _simd_f128_splitmix64(&seed);
    state->s[3] = _simd_f128_splitmix64(&seed);
}

// generate uniform [0, 1) with 106-bit entropy
SIMD_F128_INLINE simd_f128 simd_f128_prng_uniform(simd_f128_prng* state) {
    uint64_t r1 = simd_f128_prng_next(state);
    uint64_t r2 = simd_f128_prng_next(state);

    // 53 bits of mantissa * 2^-53
    double hi = (r1 >> 11) * 1.1102230246251565e-16;

    // next 53 bits * 2^-106
    double lo = (r2 >> 11) * 1.232595164407831e-32;

    return simd_f128_add(simd_f128_from_double(hi), simd_f128_from_double(lo));
}

#ifdef __cplusplus
}
#endif

#endif
