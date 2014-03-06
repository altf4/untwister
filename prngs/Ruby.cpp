/*
 * Ruby.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "Ruby.h"

Ruby::Ruby()
{
    seedValue = 0;
    mt = new MT;
    init_genrand(mt, seedValue);
}

Ruby::~Ruby() {}

const std::string Ruby::getName()
{
    return RUBY_RAND;
}

void Ruby::seed(uint32_t value)
{
    mt = new MT;
    seedValue = value;
    init_genrand(mt, value);
}

uint32_t Ruby::getSeed()
{
    return seedValue;
}

uint32_t Ruby::random()
{
    return genrand_int32(mt);
}


void Ruby::init_genrand(struct MT* mt, unsigned int s)
{
    int j;
    mt->state[0] = s & 0xffffffffU;
    for (j=1; j<N; j++) {
        mt->state[j] = (1812433253U * (mt->state[j-1] ^ (mt->state[j-1] >> 30)) + j);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array state[].                     */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt->state[j] &= 0xffffffff;  /* for >32 bit machines */
    }
    mt->left = 1;
    mt->next = mt->state + N;
}

void Ruby::next_state(struct MT *mt)
{
    unsigned int *p = mt->state;
    int j;

    mt->left = N;
    mt->next = mt->state;

    for (j=N-M+1; --j; p++)
        *p = p[M] ^ TWIST(p[0], p[1]);

    for (j=M; --j; p++)
        *p = p[M-N] ^ TWIST(p[0], p[1]);

    *p = p[M-N] ^ TWIST(p[0], mt->state[0]);
}

uint32_t Ruby::genrand_int32(struct MT *mt)
{
    /* mt must be initialized */
    unsigned int y;

    if (--mt->left <= 0) next_state(mt);
    y = *mt->next++;

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= (y >> 18);

    return y;
}
