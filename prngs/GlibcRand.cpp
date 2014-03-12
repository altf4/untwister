/*
 * GlibcRand.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "GlibcRand.h"

GlibcRand::GlibcRand()
{
    m_seedValue = 0;
}

GlibcRand::~GlibcRand() {}

const std::string GlibcRand::getName()
{
    return GLIBC_RAND;
}

void GlibcRand::seed(uint32_t value)
{
    m_seedValue = value;
    srand(value);
}

uint32_t GlibcRand::getSeed()
{
    return m_seedValue;
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
}

std::vector<uint32_t> GlibcRand::getState(void)
{
    return m_state;
}

