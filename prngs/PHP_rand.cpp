/*
    Copyright Bishop Fox 2014

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

#include "PHP_rand.h"
#include "../ConsoleColors.h"
#include <climits>

PHP_rand::PHP_rand()
{
    seedValue = 0;
}

PHP_rand::~PHP_rand() {}

const std::string PHP_rand::getName()
{
    return PHP_RAND;
}

void PHP_rand::seed(int64_t long_seed)
{
}

int64_t PHP_rand::getSeed()
{
    return seedValue;
}

uint32_t PHP_rand::random()
{
}

uint32_t PHP_rand::getStateSize(void)
{
    return PHP_RAND_STATE_SIZE;
}

void PHP_rand::setState(std::vector<uint32_t> inState)
{
    //TODO
}

std::vector<uint32_t> PHP_rand::getState(void)
{
    return m_state;
}

void PHP_rand::setEvidence(std::vector<uint32_t> evidence)
{
    m_evidence = evidence;
}

std::vector<uint32_t> PHP_rand::predictForward(uint32_t length)
{
    std::vector<uint32_t> ret;
    return ret;
}

std::vector<uint32_t> PHP_rand::predictBackward(uint32_t length)
{
    std::vector<uint32_t> ret;
    return ret;
}

bool PHP_rand::reverseToSeed(int64_t *outSeed, uint32_t depth)
{
    return false;
}

void PHP_rand::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    //TODO
}

void PHP_rand::setBounds(uint32_t min, uint32_t max)
{
    m_minBound = min;
    m_maxBound = max;
    m_isBounded = true;
}

int64_t PHP_rand::getMinSeed()
{
    return 0;
}

int64_t PHP_rand::getMaxSeed()
{
    return UINT_MAX;
}
