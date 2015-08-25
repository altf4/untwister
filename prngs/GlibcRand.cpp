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

#include "GlibcRand.h"
#include "../ConsoleColors.h"
#include <climits>

GlibcRand::GlibcRand()
{
    seedValue = 0;

    m_LSBMap.resize(GLIBC_RAND_STATE_SIZE);
}

GlibcRand::~GlibcRand() {}

const std::string GlibcRand::getName()
{
    return GLIBC_RAND;
}

void GlibcRand::seed(int64_t long_seed)
{
    uint32_t seed = (uint32_t)long_seed;
    long int i;
    int32_t word;
    int32_t *dst;
    int kc;

    /* We must make sure the seed is not 0.  Take arbitrarily 1 in this case.  */
    if (seed == 0)
        seed = 1;
    m_glibcstate[0] = seed;

    dst = m_glibcstate;
    word = seed;
    kc = 31;
    for (i = 1; i < kc; ++i)
    {
        /* This does:
        state[i] = (16807 * state[i - 1]) % 2147483647;
        but avoids overflowing 31 bits.  */
        long int hi = word / 127773;
        long int lo = word % 127773;
        word = 16807 * lo - 2836 * hi;
        if (word < 0)
            word += 2147483647;
        *++dst = word;
    }

    m_fptr = &m_glibcstate[3];
    m_rptr = &m_glibcstate[0];
    kc *= 10;
    while (--kc >= 0)
    {
        random();
    }
}

int64_t GlibcRand::getSeed()
{
    return seedValue;
}

uint32_t GlibcRand::random()
{
    int32_t result;
    int32_t *state = m_glibcstate;

    int32_t *fptr = m_fptr;
    int32_t *rptr = m_rptr;
    int32_t *end_ptr = &m_glibcstate[31];
    int32_t val;

    val = *fptr += *rptr;
    /* Chucking least random bit.  */
    result = (val >> 1) & 0x7fffffff;
    ++fptr;
    if (fptr >= end_ptr)
	{
        fptr = state;
        ++rptr;
	}
    else
	{
        ++rptr;
        if (rptr >= end_ptr)
            rptr = state;
	}
    m_fptr = fptr;
    m_rptr = rptr;

    return result;
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

/* We just have to make some guesses about the LSBs and then test those
    guesses one by one */
void GlibcRand::tune_repeatedIncrements()
{
    /* Keep tuning until no improvements are made anymore */
    bool keepGoing = true;
    while(keepGoing)
    {
        keepGoing = true;

        /* Foreach state integer, test if we can improve predictions by incrementing
            that state value.*/
        for(uint32_t i = 0; i < GLIBC_RAND_STATE_SIZE; i++)
        {
            /* Get the success rate of this state */
            std::vector<uint32_t> guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);

            int64_t sum = 0;
            for(uint32_t j = 0; j < guesses.size(); j++)
            {
                sum += std::min(guesses[j] - m_evidence[GLIBC_RAND_STATE_SIZE + j],
                    m_evidence[GLIBC_RAND_STATE_SIZE + j] - guesses[j]);
            }

            //Increment the state val
            m_state[i] += 1;

            /* Get the success rate of the new state */
            guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);

            int64_t sum_new = 0;
            for(uint32_t j = 0; j < guesses.size(); j++)
            {
                /* The guess can NEVER be bigger than the evidence */
                if(guesses[j] > m_evidence[GLIBC_RAND_STATE_SIZE + j])
                {
                    sum_new = -1;
                    break;
                }
                sum_new += std::min(guesses[j] - m_evidence[GLIBC_RAND_STATE_SIZE + j],
                    m_evidence[GLIBC_RAND_STATE_SIZE + j] - guesses[j]);
            }

            m_state[i] -= 1;
            if(sum_new < sum)
            {
                setLSB(i, 1);
            }
            else if(sum_new > sum)
            {
                //setLSB(i, 0);
                keepGoing = false;
            }
            else
            {
                keepGoing = false;
            }
        }
    }
}

bool GlibcRand::setLSB(uint32_t index, uint32_t value)
{
    if(m_LSBMap[index].m_isKnown == false)
    {
        if(index < GLIBC_RAND_STATE_SIZE)
        {
            //Set this LSB's value
            m_state[index] += value;
        }
        m_LSBMap[index].m_isKnown = true;
        m_LSBMap[index].m_LSB = value;

        //Set the opposite value for any LSBs saved as XOR'd with
        for(uint32_t i = 0; i < m_LSBMap[index].m_xorWith.size(); i++)
        {
            setLSB(m_LSBMap[index].m_xorWith[i], 1-value);
        }

        //Satisfy OR condition recursively
        if(value == 1)
        {
            for(uint32_t i = 0; i < m_LSBMap[index].m_orWith.size(); i++)
            {
                setLSB(m_LSBMap[index].m_xorWith[i], 0);
            }
        }
        if(index < GLIBC_RAND_STATE_SIZE)
        {
            return true;
        }
    }
    return false;
}

void GlibcRand::setLSBxor(uint32_t index1, uint32_t index2)
{
    /* If we don't know either value, then save this relationship */
    if(!m_LSBMap[index1].m_isKnown && !m_LSBMap[index2].m_isKnown)
    {
        m_LSBMap[index1].m_xorWith.push_back(index2);
        m_LSBMap[index2].m_xorWith.push_back(index1);
    }
    else if(m_LSBMap[index1].m_isKnown)
    {
        setLSB(index2, 1-m_LSBMap[index1].m_LSB);
    }
    else if(m_LSBMap[index2].m_isKnown)
    {
        setLSB(index1, 1-m_LSBMap[index2].m_LSB);
    }
}

void GlibcRand::setLSBor(uint32_t index1, uint32_t index2)
{
    if(!m_LSBMap[index1].m_isKnown)
    {
        m_LSBMap[index1].m_orWith.push_back(index2);
    }

    if(!m_LSBMap[index2].m_isKnown)
    {
        m_LSBMap[index2].m_orWith.push_back(index1);
    }
}

bool GlibcRand::handleRemainder(uint32_t i, std::vector<uint32_t> guesses)
{
    /* Did we learn any new information? */
    bool ret = false;

    uint32_t guess = guesses[i];
    uint32_t observed = m_evidence[GLIBC_RAND_STATE_SIZE + i];
    uint32_t diff = observed - guess;

    uint32_t diff_first, diff_second;
    /* Get Diff for first predecessor */
    if((i+1) >= GLIBC_RAND_STATE_SIZE)
    {
        diff_first = m_evidence[i+1] - guesses[i-31];
    }
    else
    {
        /* All diffs within the first 32 are "0"  */
        diff_first = 0;
    }

    /* Get Diff for second predecessor */
    if((i+29) >= GLIBC_RAND_STATE_SIZE)
    {
        diff_second = m_evidence[GLIBC_RAND_STATE_SIZE + i - 3] - guesses[i-3];
    }
    else
    {
        /* All diffs within the first 32 are "0"  */
        diff_second = 0;
    }


    /* When diff is 1, AND both predecessors are 0, (guaranteed to be the case
        in the first 1 diff) that means BOTH previous values have a set LSB */
    if((diff == 1) && (diff_first == 0) && (diff_second == 0))
    {
        /* First Value. IE: O_1*/
        ret |= setLSB(i+1, 1);
        if((i+1) >= GLIBC_RAND_STATE_SIZE)
        {
            //TODO maybe do the recursion at the end?
            handleRemainder(i-31, guesses);
        }

        /* Second Value. IE: O_31*/
        ret |= setLSB(i+29, 1);
        if((i+29) < GLIBC_RAND_STATE_SIZE)
        {
            //TODO maybe do the recursion at the end?
            handleRemainder(i-3, guesses);
        }
    }

    /* If diff of zero, and LSB of 1, then the two predecessors MUST have
        an XOR relationship */
    if((diff == 0) && (m_LSBMap[i].m_isKnown) && (m_LSBMap[i].m_LSB = 1))
    {
        setLSBxor(i-3, i-31);
    }

    return ret;
}

/* Keep hopping by 3's, checking for diff increments */
void GlibcRand::tune_chainChecking()
{
    bool keepGoing = true;
    while(keepGoing)
    {
        keepGoing = false;
        std::vector<uint32_t> guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);
        m_LSBMap.resize(GLIBC_RAND_STATE_SIZE + guesses.size());

        for(uint32_t i = 0; i < guesses.size()-3; i++)
        {
            uint32_t diff = m_evidence[GLIBC_RAND_STATE_SIZE + i] - guesses[i];
            uint32_t diff_next = m_evidence[GLIBC_RAND_STATE_SIZE + i + 3] - guesses[i+3];

            if(diff_next - diff == 1)
            {
                keepGoing |= setLSB(i+4, 1);
                keepGoing |= setLSB(GLIBC_RAND_STATE_SIZE + i, 1);
            }
            if(diff_next - diff == 0)
            {
                if((m_LSBMap[i+3].m_isKnown) && (m_LSBMap[i+3].m_LSB = 1))
                {
                    setLSBxor(i+29, i+1);
                }
            }
        }
    }
}

//XXX Maybe not possible?! http://www.mscs.dal.ca/~selinger/random/
/* Takes a pointer for efficiency (don't want to copy the state over and over) */
bool GlibcRand::isInitState(std::deque<uint32_t> *tmp_state)
{
    return false;
}

bool GlibcRand::reverseToSeed(int64_t *outSeed, uint32_t depth)
{
    /* Keep state in a deque for this, as we're going to need to go backwards a lot
        This is for efficiency only. As we might have to go very deeply backwards,
        this part has to be fast */
    std::deque<uint32_t> tmp_state;
    tmp_state.resize(GLIBC_RAND_STATE_SIZE);
    for(uint32_t i = 0; i < GLIBC_RAND_STATE_SIZE; i++)
    {
        tmp_state[i] = m_state[i];
    }

    for(uint32_t i = 0; i < depth; i++)
    {
        //o_-1 = o_30 - 0_28
        uint32_t prev = tmp_state[30] - tmp_state[27];
        tmp_state.pop_back();
        tmp_state.push_front(prev);

        if(isInitState(&tmp_state))
        {
            *outSeed = tmp_state[0];
            return true;
        }
    }
    return false;
}

/* In glibc-rand, the rand() function chops off the LSB of the computed value.
    This makes reversing it annoying, but not impossible. */
void GlibcRand::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    tune_chainChecking();
    tune_repeatedIncrements();
}

void GlibcRand::setBounds(uint32_t min, uint32_t max)
{
    //Setting bounds is unsupported in gblibc, so do nothing here. In fact, this should not get called.
}

int64_t GlibcRand::getMinSeed()
{
    return 0;
}

int64_t GlibcRand::getMaxSeed()
{
    return UINT_MAX;
}
