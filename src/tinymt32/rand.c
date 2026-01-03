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

__attribute__((constructor))
static void init_prng(void)
{
	srand(1);
}
