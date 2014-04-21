/*
 * GlibcRand.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "GlibcRand.h"
#include <algorithm>

GlibcRand::GlibcRand()
{
    seedValue = 0;
}

GlibcRand::~GlibcRand() {}

const std::string GlibcRand::getName()
{
    return GLIBC_RAND;
}

void GlibcRand::seed(uint32_t value)
{
    seedValue = value;
    srand(value);
}

uint32_t GlibcRand::getSeed()
{
    return seedValue;
}

uint32_t GlibcRand::random()
{
    return rand();
}

uint32_t GlibcRand::getStateSize(void)
{
    return GLIBC_RAND_STATE_SIZE;
}

void GlibcRand::setState(std::vector<uint32_t> inState)
{
    m_state = inState;
    m_state.resize(GLIBC_RAND_STATE_SIZE, 0);

    /* Shift left one bit to return to mod 2^32.
        Of course, we'll be missing the original LSB,
        so we'll have to go fishing for it later. */
    for(uint32_t i = 0; i < m_state.size(); i++)
    {
        m_state[i] = m_state[i]<<1;
    }
}

std::vector<uint32_t> GlibcRand::getState(void)
{
    return m_state;
}

std::vector<uint32_t> GlibcRand::predictForward(uint32_t length)
{
    std::vector<uint32_t> running_state = m_state;
    running_state.resize(GLIBC_RAND_STATE_SIZE + length);

    std::vector<uint32_t> ret;
    
    /* There is a more memory efficient way to do this. With a 
        linked list for the state. But meh.  */
    for(uint32_t i = 32; i < length + 32; i++)
    {
        uint32_t val = running_state[i-31] + running_state[i-3];
        running_state[i] = val;
        ret.push_back((val >> 1) & 0x7fffffff);
    }

    return ret;
}

std::vector<uint32_t> GlibcRand::predictBackward(uint32_t length)
{
    std::vector<uint32_t> running_state = m_state;
    /* Reverse order for simplicity. Deal with it. */
    std::reverse(running_state.begin(), running_state.end());
    running_state.resize(GLIBC_RAND_STATE_SIZE + length);

    std::vector<uint32_t> ret;
    
    /* There is a more memory efficient way to do this. With a 
        linked list for the state. But meh.  */
    for(uint32_t i = GLIBC_RAND_STATE_SIZE; i < length + GLIBC_RAND_STATE_SIZE; i++)
    {
        uint32_t val = running_state[i-31] - running_state[i-28];
        running_state[i] = val;
        ret.push_back((val >> 1) & 0x7fffffff);
    }

    /* Reverse order for simplicity. Deal with it. */
    std::reverse(ret.begin(), ret.end());
    return ret;
}

void GlibcRand::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{

}

