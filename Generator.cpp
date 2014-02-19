/*
    Copyright Dan Petro, 2014

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "Generator.h"
#include <iostream>

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

using namespace std;

typedef void (Generator::*GeneratorSeedFn)(uint32_t seed);
typedef uint32_t (Generator::*GeneratorRandFn)(void);

GeneratorSeedFn untwister_srand;
GeneratorRandFn untwister_rand;

Generator::Generator(std::string rng)
{
    if(rng.compare("mt19937") == 0)
    {
        untwister_srand = &Generator::mt19937_srand;
        untwister_rand = &Generator::mt19937_rand;
    }
    else if(rng.compare("glibc-rand") == 0)
    {
        untwister_srand = &Generator::glibc_rand_srand;
        untwister_rand = &Generator::glibc_rand_rand;
    }
    else if(rng.compare("mt19937-php") == 0)
    {
        untwister_srand = &Generator::mt19937_php_srand;
        untwister_rand = &Generator::mt19937_php_rand;        
    }
    else
    {
        //Error case
        cerr << "ERROR: Unknown rng type selected" << endl;
        exit(EXIT_FAILURE);
    }

    mt19937_next = NULL;
}

void Generator::Seed(uint32_t seed)
{
    CALL_MEMBER_FN(*this, untwister_srand)(seed);
}

uint32_t Generator::Random()
{
    return CALL_MEMBER_FN(*this, untwister_rand)();
}

void Generator::mt19937_srand(uint32_t seed)
{
    mt19937_generator.seed(seed);
}

uint32_t Generator::mt19937_rand()
{
    return mt19937_generator();
}

//mt19937-php

#define hiBit(u) ((u) & 0x80000000U) /* mask all but highest bit of u */
#define loBit(u) ((u) & 0x00000001U) /* mask all but lowest bit of u */
#define loBits(u) ((u) & 0x7FFFFFFFU) /* mask the highest bit of u */
#define mixBits(u, v) (hiBit(u)|loBits(v)) /* move hi bit of u to hi bit of v */
#define twist(m,u,v) (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(loBit(u))) & 0x9908b0dfU))

void Generator::php_mt_initialize(uint32_t seed, uint32_t *state)
{
    register uint32_t *s = state;
    register uint32_t *r = state;
    register int i = 1;

    *s++ = seed & 0xffffffffU;
    for( ; i < 624; ++i ) 
    {
        *s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
        r++;
    }
}

void Generator::php_mt_reload()
{
    register uint32_t *state = mt19937_state;
    register uint32_t *p = state;
    register int i;

    int M = 397;
    int N = 624;

    for (i = N - M; i--; ++p)
        *p = twist(p[M], p[0], p[1]);
    for (i = M; --i; ++p)
        *p = twist(p[M-N], p[0], p[1]);
    *p = twist(p[M-N], p[0], state[0]);
    mt19937_left = (int)N;
    mt19937_next = state;
}

void Generator::mt19937_php_srand(uint32_t seed)
{
    php_mt_initialize(seed, mt19937_state);
    php_mt_reload();
}

uint32_t Generator::mt19937_php_rand()
{
    register uint32_t s1;

    if (mt19937_left == 0)
    {
        php_mt_reload();
    }
    --mt19937_left;

    s1 = *mt19937_next++;
    s1 ^= (s1 >> 11);
    s1 ^= (s1 << 7) & 0x9d2c5680U;
    s1 ^= (s1 << 15) & 0xefc60000U;
    return ( s1 ^ (s1 >> 18) );    
}

//glibc-rand

void Generator::glibc_rand_srand(uint32_t seed)
{
    srand(seed);
}

uint32_t Generator::glibc_rand_rand()
{
    return rand();
}

