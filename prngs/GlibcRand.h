/*
 * GlibcRand.h
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#ifndef GLIBCRAND_H_
#define GLIBCRAND_H_

#include <algorithm>
#include <random>
#include <deque>
#include "PRNG.h"
#include "LSBState.h"

/*
   Copyright (C) 1995, 2005, 2009 Free Software Foundation
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/*
   Copyright (C) 1983 Regents of the University of California.
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   4. Neither the name of the University nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.*/

/*
 * This is derived from the Berkeley source:
 *  @(#)random.c    5.5 (Berkeley) 7/6/88
 * It was reworked for the GNU C Library by Roland McGrath.
 * Rewritten to be reentrant by Ulrich Drepper, 1995
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>


/* An improved random number generation package.  In addition to the standard
   rand()/srand() like interface, this package also has a special state info
   interface.  The initstate() routine is called with a seed, an array of
   bytes, and a count of how many bytes are being passed in; this array is
   then initialized to contain information for random number generation with
   that much state information.  Good sizes for the amount of state
   information are 32, 64, 128, and 256 bytes.  The state can be switched by
   calling the setstate() function with the same array as was initialized
   with initstate().  By default, the package runs with 128 bytes of state
   information and generates far better random numbers than a linear
   congruential generator.  If the amount of state information is less than
   32 bytes, a simple linear congruential R.N.G. is used.  Internally, the
   state information is treated as an array of longs; the zeroth element of
   the array is the type of R.N.G. being used (small integer); the remainder
   of the array is the state information for the R.N.G.  Thus, 32 bytes of
   state information will give 7 longs worth of state information, which will
   allow a degree seven polynomial.  (Note: The zeroth word of state
   information also has some other information stored in it; see setstate
   for details).  The random number generation technique is a linear feedback
   shift register approach, employing trinomials (since there are fewer terms
   to sum up that way).  In this approach, the least significant bit of all
   the numbers in the state table will act as a linear feedback shift register,
   and will have period 2^deg - 1 (where deg is the degree of the polynomial
   being used, assuming that the polynomial is irreducible and primitive).
   The higher order bits will have longer periods, since their values are
   also influenced by pseudo-random carries out of the lower bits.  The
   total period of the generator is approximately deg*(2**deg - 1); thus
   doubling the amount of state information has a vast influence on the
   period of the generator.  Note: The deg*(2**deg - 1) is an approximation
   only good for large deg, when the period of the shift register is the
   dominant factor.  With deg equal to seven, the period is actually much
   longer than the 7*(2**7 - 1) predicted by this formula.  */



/* For each of the currently supported random number generators, we have a
   break value on the amount of state information (you need at least this many
   bytes of state info to support this random number generator), a degree for
   the polynomial (actually a trinomial) that the R.N.G. is based on, and
   separation between the two lower order coefficients of the trinomial.  */

/* Linear congruential.  */
#define TYPE_0      0
#define BREAK_0     8
#define DEG_0       0
#define SEP_0       0

/* x**7 + x**3 + 1.  */
#define TYPE_1      1
#define BREAK_1     32
#define DEG_1       7
#define SEP_1       3

/* x**15 + x + 1.  */
#define TYPE_2      2
#define BREAK_2     64
#define DEG_2       15
#define SEP_2       1

/* x**31 + x**3 + 1.  */
#define TYPE_3      3
#define BREAK_3     128
#define DEG_3       31
#define SEP_3       3

/* x**63 + x + 1.  */
#define TYPE_4      4
#define BREAK_4     256
#define DEG_4       63
#define SEP_4       1


/* Array versions of the above information to make code run faster.
   Relies on fact that TYPE_i == i.  */

#define MAX_TYPES   5   /* Max number of types above.  */

struct random_poly_info
{
  int seps[MAX_TYPES];
  int degrees[MAX_TYPES];
};


struct random_data
{
    int32_t *fptr;      /* Front pointer.  */
    int32_t *rptr;      /* Rear pointer.  */
    int32_t *state;     /* Array of state values.  */
    int rand_type;      /* Type of random number generator.  */
    int rand_deg;       /* Degree of random number generator.  */
    int rand_sep;       /* Distance between front and rear.  */
    int32_t *end_ptr;   /* Pointer behind state table.  */
};


static const struct random_poly_info random_poly_info =
{
    { SEP_0, SEP_1, SEP_2, SEP_3, SEP_4 },
    { DEG_0, DEG_1, DEG_2, DEG_3, DEG_4 }
};


/* Initially, everything is set up as if from:
    initstate(1, randtbl, 128);
   Note that this initialization takes advantage of the fact that srandom
   advances the front and rear pointers 10*rand_deg times, and hence the
   rear pointer which starts at 0 will also end up at zero; thus the zeroth
   element of the state information, which contains info about the current
   position of the rear pointer is just
    (MAX_TYPES * (rptr - state)) + TYPE_3 == TYPE_3.  */

static int32_t randtbl[DEG_3 + 1] =
{
    TYPE_3,

    -1726662223, 379960547, 1735697613, 1040273694, 1313901226,
    1627687941, -179304937, -2073333483, 1780058412, -1989503057,
    -615974602, 344556628, 939512070, -1249116260, 1507946756,
    -812545463, 154635395, 1388815473, -1926676823, 525320961,
    -1009028674, 968117788, -123449607, 1284210865, 435012392,
    -2017506339, -911064859, -370259173, 1132637927, 1398500161,
    -205601318,
};

static struct random_data unsafe_state =
{
/* FPTR and RPTR are two pointers into the state info, a front and a rear
   pointer.  These two pointers are always rand_sep places aparts, as they
   cycle through the state information.  (Yes, this does mean we could get
   away with just one pointer, but the code for random is more efficient
   this way).  The pointers are left positioned as they would be from the call:
    initstate(1, randtbl, 128);
   (The position of the rear pointer, rptr, is really 0 (as explained above
   in the initialization of randtbl) because the state table pointer is set
   to point to randtbl[1] (as explained below).)  */

    .fptr = &randtbl[SEP_3 + 1],
    .rptr = &randtbl[1],

/* The following things are the pointer to the state information table,
   the type of the current generator, the degree of the current polynomial
   being used, and the separation between the two pointers.
   Note that for efficiency of random, we remember the first location of
   the state information, not the zeroth.  Hence it is valid to access
   state[-1], which is used to store the type of the R.N.G.
   Also, we remember the last location, since this is more efficient than
   indexing every time to find the address of the last element to see if
   the front and rear pointers have wrapped.  */

    .state = &randtbl[1],

    .rand_type = TYPE_3,
    .rand_deg = DEG_3,
    .rand_sep = SEP_3,

    .end_ptr = &randtbl[sizeof (randtbl) / sizeof (randtbl[0])]
};

/* Untwister Constants */
static const std::string GLIBC_RAND = "glibc-rand";
static const uint32_t GLIBC_RAND_STATE_SIZE = 32;


class GlibcRand: public PRNG
{
public:
    GlibcRand();
    virtual ~GlibcRand();

    const std::string getName(void);
    void seed(uint32_t value);
    uint32_t getSeed(void);
    uint32_t random(void);

    uint32_t seedValue;
    uint32_t getStateSize(void);
    void setState(std::vector<uint32_t> inState);
    std::vector<uint32_t> getState(void);

    void setEvidence(std::vector<uint32_t>);

    std::vector<uint32_t> predictForward(uint32_t);
    std::vector<uint32_t> predictBackward(uint32_t);

    bool setLSB(uint32_t index, uint32_t value);
    void setLSBxor(uint32_t index1, uint32_t index2);
    void setLSBor(uint32_t index1, uint32_t index2);
    bool handleRemainder(uint32_t index, std::vector<uint32_t>);

    void tune(std::vector<uint32_t>, std::vector<uint32_t>);
    void tune_repeatedIncrements();
    void tune_chainChecking();

    bool isInitState(std::deque<uint32_t> *);

    bool reverseToSeed(uint32_t *, uint32_t);

private:
    /* Keeps track of what LSBs are known */
    std::vector<LSBState> m_LSBMap;

    /* Glibc rand() implementation */
    int m_rand();
    void m_srand(unsigned int seed);
    long int __random();
    void __srandom(unsigned int x);
    int __srandom_r(unsigned int seed, struct random_data *buf);
    int __initstate_r(unsigned int seed, char *arg_state, size_t n, struct random_data *buf);
    int __setstate_r(char *arg_state, struct random_data *buf);
    int __random_r(struct random_data *buf, int32_t *result);

};

#endif /* GLIBCRAND_H_ */
