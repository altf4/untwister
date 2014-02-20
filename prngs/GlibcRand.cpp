/*
 * GlibcRand.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "GlibcRand.h"

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
