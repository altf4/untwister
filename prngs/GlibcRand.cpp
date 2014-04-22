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

    m_LSBMap.resize(GLIBC_RAND_STATE_SIZE);
    for(uint32_t i = 0; i < m_LSBMap.size(); i++)
    {
        m_LSBMap[i] = LSB_UNKNOWN;
    }
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

void GlibcRand::setEvidence(std::vector<uint32_t> evidence)
{
    m_evidence = evidence;
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

#include <iostream>

/* We just have to make some guesses about the LSBs and then test those 
    guesses one by one */
void GlibcRand::tune_repeatedIncrements()
{
    /* Keep tuning until no improvements are made anymore */
    bool keepGoing = true;
    while(keepGoing)
    {
        keepGoing = false;

        /* Foreach state integer, test if we can improve predictions by incrementing
            that state value.*/
        for(uint32_t i = 0; i < GLIBC_RAND_STATE_SIZE; i++)
        {
            /* Get the success rate of this state */
            std::vector<uint32_t> guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);

            double correct = 0;
            for(uint32_t j = 0; j < guesses.size(); j++)
            {
                if(guesses[j] == m_evidence[GLIBC_RAND_STATE_SIZE + j])
                {
                    correct += 1;
                }
            }
            double success_rate = (correct * 100) / guesses.size();

            //Increment the state val
            m_state[i] += 1;
            
            /* Get the success rate of the new state */
            guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);

            correct = 0;
            for(uint32_t j = 0; j < guesses.size(); j++)
            {
                if(guesses[j] == m_evidence[GLIBC_RAND_STATE_SIZE + j])
                {
                    correct += 1;
                }
            }

            double new_success_rate = (correct * 100) / guesses.size();

            if(new_success_rate > success_rate)
            {
                std::cout << "Success: " << i << " " << success_rate << " -> " << new_success_rate << std::endl;
                m_LSBMap[i] = LSB_CORRECT;
                keepGoing = true;
            }
            else
            {
                m_state[i] -= 1;
            }
        }
    }
}

/* Nearly equals operator. Returns true if x and y are nearly equal,
    plus or minor the fudge_factor, but NOT actually equal! */
bool isNearlyEqual(uint32_t x, uint32_t y, uint32_t fudge_factor)
{
    std::cout << "Fuzzy: " << x << " : " << y << " : " << fudge_factor << std::endl;

    if(x == y)
        return false;

    if(x - y <= fudge_factor)
        return true;
    
    if(y - x <= fudge_factor)
        return true;

    return false;
}

/* Test for equality using a +/- X margin of error. If the expected value is close
    to what we predicted, then it's probably due to an incorrect LSB. */
void GlibcRand::tune_fuzzyGuessing()
{
    std::vector<uint32_t> guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);

    double correct = 0;
    for(uint32_t i = 0; (i < guesses.size()) && (i < GLIBC_RAND_STATE_SIZE); i++)
    {
        /* Increase fudge factor every 32 ints */
        //TODO look further into calculating this
        uint32_t fudge_factor = ((i / GLIBC_RAND_STATE_SIZE) * 2) + 4;
        uint32_t guess = guesses[i];
        uint32_t observed = m_evidence[GLIBC_RAND_STATE_SIZE + i];

        if(isNearlyEqual(guess, observed, fudge_factor))
        {
            /* If only one of the responsible LSBs is wrong,
             then we can easily recover the other */
            bool isFirstLSBCorrect = false;
            if(i+1 < GLIBC_RAND_STATE_SIZE)
            {
                if(m_LSBMap[i+1] == LSB_CORRECT)
                    isFirstLSBCorrect = true;
            }

            bool isSecondLSBCorrect = false;
            if(i+29 < GLIBC_RAND_STATE_SIZE)
            {
                if(m_LSBMap[i+29] == LSB_CORRECT)
                    isSecondLSBCorrect = true;
            }

            /* If exactly one of the LSBs is wrong... */
            if(isFirstLSBCorrect ^ isSecondLSBCorrect)
            {
                //XXX PICK UP HERE
            }

            std::cout.setf(std::ios::boolalpha);
            std::cout << "Fuzzy: " << isFirstLSBCorrect << ", " << isSecondLSBCorrect << std::endl;

            correct += 1;
        }
        else
        {
            //TODO mark LSBs as wrong
        }
    }

    double success_rate = (correct * 100) / guesses.size();
    std::cout << "Fuzzy Success Rate: " << success_rate << std::endl;
}

/* Get some easy wins in the first 32 ints. 
    If we get a perfect guess, then that means both of 
    the LSBs responsible are correct */
void GlibcRand::tune_checkLSBs()
{
    std::vector<uint32_t> guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);
    for(uint32_t i = 0; (i < guesses.size()) && (i < GLIBC_RAND_STATE_SIZE); i++)
    {
        /* Increase fudge factor every 32 ints */
        //TODO look further into calculating this
        uint32_t guess = guesses[i];
        uint32_t observed = m_evidence[GLIBC_RAND_STATE_SIZE + i];

        if(guess == observed)
        {
            if((i+1) < GLIBC_RAND_STATE_SIZE)
            {
                m_LSBMap[i+1] = LSB_CORRECT;
            }
            if((i+29) < GLIBC_RAND_STATE_SIZE)
            {
                m_LSBMap[i+29] = LSB_CORRECT;
            }
        }
    }
}

/* In glibc-rand, the rand() function chops off the LSB of the computed value. 
    This makes reversing it annoying, but not impossible. */
void GlibcRand::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    tune_repeatedIncrements();
    //tune_checkLSBs();
    //tune_fuzzyGuessing();

    for(uint32_t i = 0; i < m_LSBMap.size(); i++)
    {
        std::cout << "LSBMap: " << i << ": " << m_LSBMap[i] << std::endl;
    }
}

