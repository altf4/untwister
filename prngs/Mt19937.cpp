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

#include "Mt19937.h"
#include "../ConsoleColors.h"
#include <climits>

Mt19937::Mt19937()
{
    seedValue = generator.default_seed;
}

Mt19937::~Mt19937() {}

const std::string Mt19937::getName()
{
    return MT19937;
}

void Mt19937::seed(int64_t value)
{
    seedValue = (uint32_t)value;
    this->generator.seed(seedValue);
}

int64_t Mt19937::getSeed()
{
    return seedValue;
}

uint32_t Mt19937::random(void)
{
    return this->generator();
}

uint32_t Mt19937::getStateSize(void)
{
    return MT19937_STATE_SIZE;
}

void Mt19937::setState(std::vector<uint32_t> inState)
{
    m_state = inState;
    m_state.resize(MT19937_STATE_SIZE, 0);
}

std::vector<uint32_t> Mt19937::getState(void)
{
    return m_state;
}

std::vector<uint32_t> Mt19937::predictForward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

std::vector<uint32_t> Mt19937::predictBackward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}


bool Mt19937::reverseToSeed(int64_t *outSeed, uint32_t depth)
{
    //TODO
    return false;
}

void Mt19937::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    //TODO
}

void Mt19937::setEvidence(std::vector<uint32_t>)
{

}

void Mt19937::setBounds(uint32_t min, uint32_t max)
{
    //Setting bounds is unsupported in C++ rand, so do nothing here. In fact, this should not get called.
}

int64_t Mt19937::getMinSeed()
{
    return 0;
}

int64_t Mt19937::getMaxSeed()
{
    return UINT_MAX;
}
