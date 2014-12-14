/*
    Copyright Dan Petro, moloch 2014

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

void GlibcRand::seed(uint32_t value)
{
    seedValue = value;
    m_srand(value);
}

uint32_t GlibcRand::getSeed()
{
    return seedValue;
}

uint32_t GlibcRand::random()
{
    return m_rand();
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

            uint64_t sum = 0;
            for(uint32_t j = 0; j < guesses.size(); j++)
            {
                sum += std::min(guesses[j] - m_evidence[GLIBC_RAND_STATE_SIZE + j],
                    m_evidence[GLIBC_RAND_STATE_SIZE + j] - guesses[j]);
            }

            //Increment the state val
            m_state[i] += 1;

            /* Get the success rate of the new state */
            guesses = this->predictForward(m_evidence.size() - GLIBC_RAND_STATE_SIZE);

            uint64_t sum_new = 0;
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

bool GlibcRand::reverseToSeed(uint32_t *outSeed, uint32_t depth)
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

/* GNU Glibc RAND Implementation */

int GlibcRand::m_rand()
{
    return (int) __random();
}

void GlibcRand::m_srand(unsigned int seed)
{
    __srandom(seed);
}


long int GlibcRand::__random()
{
    int32_t retval;

    //__libc_lock_lock (lock);
    (void) __random_r(&unsafe_state, &retval);
    //__libc_lock_unlock (lock);

    return retval;
}

/* Initialize the random number generator based on the given seed.  If the
   type is the trivial no-state-information type, just remember the seed.
   Otherwise, initializes state[] based on the given "seed" via a linear
   congruential generator.  Then, the pointers are set to known locations
   that are exactly rand_sep places apart.  Lastly, it cycles the state
   information a given number of times to get rid of any initial dependencies
   introduced by the L.C.R.N.G.  Note that the initialization of randtbl[]
   for default usage relies on values produced by this routine.  */
void GlibcRand::__srandom(unsigned int x)
{
      //__libc_lock_lock (lock);
      (void) __srandom_r(x, &unsafe_state);
      //__libc_lock_unlock (lock);
}


/* Initialize the random number generator based on the given seed.  If the
   type is the trivial no-state-information type, just remember the seed.
   Otherwise, initializes state[] based on the given "seed" via a linear
   congruential generator.  Then, the pointers are set to known locations
   that are exactly rand_sep places apart.  Lastly, it cycles the state
   information a given number of times to get rid of any initial dependencies
   introduced by the L.C.R.N.G.  Note that the initialization of randtbl[]
   for default usage relies on values produced by this routine.  */
int GlibcRand::__srandom_r(unsigned int seed, struct random_data *buf)
{
    int type;
    int32_t *state;
    long int i;
    int32_t word;
    int32_t *dst;
    int kc;

    if (buf == NULL)
    return -1; // goto fail
    type = buf->rand_type;
    if ((unsigned int) type >= MAX_TYPES)
    return -1; // goto fail

    state = buf->state;
    /* We must make sure the seed is not 0.  Take arbitrarily 1 in this case.  */
    if (seed == 0)
    seed = 1;
    state[0] = seed;
    if (type == TYPE_0)
    return 0; // goto done

    dst = state;
    word = seed;
    kc = buf->rand_deg;
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

    buf->fptr = &state[buf->rand_sep];
    buf->rptr = &state[0];
    kc *= 10;
    while (--kc >= 0)
    {
        int32_t discard;
        (void) __random_r (buf, &discard);
    }
    return 0;

/*
 done:
  return 0;

 fail:
  return -1;
*/
}


/* Initialize the state information in the given array of N bytes for
   future random number generation.  Based on the number of bytes we
   are given, and the break values for the different R.N.G.'s, we choose
   the best (largest) one we can and set things up for it.  srandom is
   then called to initialize the state information.  Note that on return
   from srandom, we set state[-1] to be the type multiplexed with the current
   value of the rear pointer; this is so successive calls to initstate won't
   lose this information and will be able to restart with setstate.
   Note: The first thing we do is save the current state, if any, just like
   setstate so that it doesn't matter when initstate is called.
   Returns 0 on success, non-zero on failure.  */
int GlibcRand::__initstate_r(unsigned int seed, char *arg_state, size_t n, struct random_data *buf)
{
  if (buf == NULL)
    return -1; // goto fail

  int32_t *old_state = buf->state;
  if (old_state != NULL)
    {
      int old_type = buf->rand_type;
      if (old_type == TYPE_0)
    old_state[-1] = TYPE_0;
      else
    old_state[-1] = (MAX_TYPES * (buf->rptr - old_state)) + old_type;
    }

  int type;
  if (n >= BREAK_3)
    type = n < BREAK_4 ? TYPE_3 : TYPE_4;
  else if (n < BREAK_1)
    {
      if (n < BREAK_0)
        return -1; // goto fail

      type = TYPE_0;
    }
  else
    type = n < BREAK_2 ? TYPE_1 : TYPE_2;

  int degree = random_poly_info.degrees[type];
  int separation = random_poly_info.seps[type];

  buf->rand_type = type;
  buf->rand_sep = separation;
  buf->rand_deg = degree;
  int32_t *state = &((int32_t *) arg_state)[1]; /* First location.  */
  /* Must set END_PTR before srandom.  */
  buf->end_ptr = &state[degree];

  buf->state = state;

  __srandom_r (seed, buf);

  state[-1] = TYPE_0;
  if (type != TYPE_0)
    state[-1] = (buf->rptr - state) * MAX_TYPES + type;

  return 0;

/* fail:
  __set_errno (EINVAL);
  return -1;
*/
}

/* Restore the state from the given state array.
   Note: It is important that we also remember the locations of the pointers
   in the current state information, and restore the locations of the pointers
   from the old state information.  This is done by multiplexing the pointer
   location into the zeroth word of the state information. Note that due
   to the order in which things are done, it is OK to call setstate with the
   same state as the current state
   Returns 0 on success, non-zero on failure.  */
int GlibcRand::__setstate_r(char *arg_state, struct random_data *buf)
{
  int32_t *new_state = 1 + (int32_t *) arg_state;
  int type;
  int old_type;
  int32_t *old_state;
  int degree;
  int separation;

  if (arg_state == NULL || buf == NULL)
    return -1; // goto fail

  old_type = buf->rand_type;
  old_state = buf->state;
  if (old_type == TYPE_0)
    old_state[-1] = TYPE_0;
  else
    old_state[-1] = (MAX_TYPES * (buf->rptr - old_state)) + old_type;

  type = new_state[-1] % MAX_TYPES;
  if (type < TYPE_0 || type > TYPE_4)
    return -1; // goto fail

  buf->rand_deg = degree = random_poly_info.degrees[type];
  buf->rand_sep = separation = random_poly_info.seps[type];
  buf->rand_type = type;

  if (type != TYPE_0)
    {
      int rear = new_state[-1] / MAX_TYPES;
      buf->rptr = &new_state[rear];
      buf->fptr = &new_state[(rear + separation) % degree];
    }
  buf->state = new_state;
  /* Set end_ptr too.  */
  buf->end_ptr = &new_state[degree];

  return 0;

/*
 fail:
  __set_errno (EINVAL);
  return -1;
*/
}

/* If we are using the trivial TYPE_0 R.N.G., just do the old linear
   congruential bit.  Otherwise, we do our fancy trinomial stuff, which is the
   same in all the other cases due to all the global variables that have been
   set up.  The basic operation is to add the number at the rear pointer into
   the one at the front pointer.  Then both pointers are advanced to the next
   location cyclically in the table.  The value returned is the sum generated,
   reduced to 31 bits by throwing away the "least random" low bit.
   Note: The code takes advantage of the fact that both the front and
   rear pointers can't wrap on the same call by not testing the rear
   pointer if the front one has wrapped.  Returns a 31-bit random number.  */

int GlibcRand::__random_r(struct random_data *buf, int32_t *result)
{
    int32_t *state;

    if (buf == NULL || result == NULL)
        return -1; // goto fail

    state = buf->state;

    if (buf->rand_type == TYPE_0)
    {
        int32_t val = state[0];
        val = ((state[0] * 1103515245) + 12345) & 0x7fffffff;
        state[0] = val;
        *result = val;
    }
    else
    {
        int32_t *fptr = buf->fptr;
        int32_t *rptr = buf->rptr;
        int32_t *end_ptr = buf->end_ptr;
        int32_t val;

        val = *fptr += *rptr;
        /* Chucking least random bit.  */
        *result = (val >> 1) & 0x7fffffff;
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
        buf->fptr = fptr;
        buf->rptr = rptr;
    }
    return 0;
/*
 fail:
  __set_errno (EINVAL);
  return -1;
*/
}
