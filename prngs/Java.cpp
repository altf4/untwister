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

#include "Java.h"
#include "../ConsoleColors.h"
#include <climits>

Java::Java()
{
    m_seedValue = 0;
    m_originalSeed = 0;
}

Java::~Java() {}

const std::string Java::getName()
{
    return JAVA;
}

void Java::seed(int64_t value)
{
    m_originalSeed = value;
    m_seedValue = (value ^ 0x5DEECE66DL) & ((1L << 48) - 1);
}

int32_t Java::next(int32_t bits)
{
    /* Update the seed */
    m_seedValue = (m_seedValue * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    /* Return bits of the seed, masked out. */
    return (int32_t)(m_seedValue >> (48 - bits));
}

int64_t Java::getSeed()
{
    return m_originalSeed;
}

uint32_t Java::random(void)
{
    if(m_isBounded)
    {
        /* Java expects the upper bound to be exclusive, but untwister is inclusive.
            So we need to add one to the bound here to compensate. */
        uint32_t bound = m_maxBound - m_minBound + 1;

        if ((bound & -bound) == bound)  // i.e., bound is a power of 2
        {
            return (int32_t)((bound * (int64_t)next(31)) >> 31);
        }

        int32_t bits, val;
        do {
            bits = next(31);
            val = bits % bound;
        } while (bits - val + (bound-1) < 0);
        return val + m_minBound;
    }
    else
    {
        return next(32);
    }
}

uint32_t Java::getStateSize(void)
{
    return JAVA_STATE_SIZE;
}

void Java::setState(std::vector<uint32_t> inState)
{
    m_state = inState;
    m_state.resize(JAVA_STATE_SIZE, 0);
}

std::vector<uint32_t> Java::getState(void)
{
    return m_state;
}

std::vector<uint32_t> Java::predictForward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

std::vector<uint32_t> Java::predictBackward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

bool Java::reverseToSeed(int64_t *outSeed, uint32_t depth)
{
    //TODO
    return false;
}

void Java::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    //TODO
}

void Java::setEvidence(std::vector<uint32_t>)
{

}

void Java::setBounds(uint32_t min, uint32_t max)
{
    m_minBound = min;
    m_maxBound = max;
    m_isBounded = true;
}

int64_t Java::getMinSeed()
{
    return LLONG_MIN;
}

int64_t Java::getMaxSeed()
{
    return LLONG_MAX;
}
