/*
    Copyright Bishop Fox, 2014

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

#include "Ruby.h"
#include "../ConsoleColors.h"
#include <iostream>
#include <climits>

Ruby::Ruby()
{
    m_isBounded = false;
    seedValue = 0;
    m_mt = new MT;
    init_genrand(m_mt, seedValue);
}

Ruby::~Ruby()
{
    delete m_mt;
    m_mt = NULL;
}

const std::string Ruby::getName()
{
    return RUBY_RAND;
}

void Ruby::seed(int64_t value)
{
    delete m_mt;
    m_mt = new MT;
    seedValue = (uint32_t)value;
    init_genrand(m_mt, seedValue);
}

int64_t Ruby::getSeed()
{
    return seedValue;
}

uint32_t Ruby::random()
{
    if(m_isBounded)
    {
        /* generate a number between 0 and (max-min), then scale it back up */
        uint32_t limit = m_maxBound - m_minBound;

        /* Ruby does an algorithm of retries within a power of two bound */
        uint32_t mask = make_mask(limit);
        while(true)
        {
            uint32_t val = genrand_int32(m_mt);
            val &= mask;
            if(val < limit)
            {
                return val + m_minBound;
            }
        }
    }
    else
    {
        return genrand_int32(m_mt);
    }
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

uint32_t Ruby::getStateSize(void)
{
    return RUBY_STATE_SIZE;
}

void Ruby::setState(std::vector<uint32_t> inState)
{
    m_state = inState;
    m_state.resize(RUBY_STATE_SIZE, 0);
}

std::vector<uint32_t> Ruby::getState(void)
{
    return m_state;
}

std::vector<uint32_t> Ruby::predictForward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

std::vector<uint32_t> Ruby::predictBackward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

bool Ruby::reverseToSeed(int64_t *outSeed, uint32_t depth)
{
    return false;
}

void Ruby::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    //TODO
}

void Ruby::setEvidence(std::vector<uint32_t>)
{

}

uint32_t Ruby::make_mask(uint32_t x)
{
    x = x | x >> 1;
    x = x | x >> 2;
    x = x | x >> 4;
    x = x | x >> 8;
    x = x | x >> 16;
    return x;
}

void Ruby::setBounds(uint32_t min, uint32_t max)
{
    m_minBound = min;
    m_maxBound = max;
    m_isBounded = true;
}

int64_t Ruby::getMinSeed()
{
    return 0;
}

int64_t Ruby::getMaxSeed()
{
    return UINT_MAX;
}
