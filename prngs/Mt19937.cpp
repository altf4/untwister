/*
 * Mt19937.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "Mt19937.h"

Mt19937::Mt19937()
{
    seedValue = generator.default_seed;
}

Mt19937::~Mt19937() {}

const std::string Mt19937::getName()
{
    return MT19937;
}

void Mt19937::seed(uint32_t value)
{
    seedValue = value;
    this->generator.seed(value);
}

uint32_t Mt19937::getSeed()
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


bool Mt19937::reverseToSeed(uint32_t *outSeed, uint32_t depth)
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
