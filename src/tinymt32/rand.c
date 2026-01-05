#include <stdlib.h>
#include "tinymt32.h"

static tinymt32_t random;

void srand(unsigned int seed)
{
	tinymt32_init(&random, seed);
}

int rand(void)
{
	return tinymt32_generate_uint32(&random) & 0x7fffffff;
}

/// @brief Mostly unbiased random number generation within 0 and range-1
/// @param range maximum value + 1
/// @return Random int between 0 and range-1
unsigned bounded_rand(unsigned range)
{
    for (unsigned x, r;;)
        if (x = rand(), r = x % range, x - r <= -range)
            return r;
}

__attribute__((constructor))
static void init_prng(void)
{
	srand(1);
}
