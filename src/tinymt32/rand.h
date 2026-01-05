#ifndef RAND_H
#define RAND_H

#include "tinymt32.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the PRNG with a seed.
 * @param seed The seed value.
 */
void srand(unsigned int seed);

/**
 * Generate a random integer in the range [0, 0x7fffffff].
 * @return Random integer.
 */
int rand(void);

/// @brief Mostly unbiased random number generation within 0 and range-1
/// @param range maximum value + 1
/// @return Random int between 0 and range-1
unsigned bounded_rand(unsigned range);

#ifdef __cplusplus
}
#endif

#endif // RAND_H
