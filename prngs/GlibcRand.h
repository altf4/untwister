/*
 * GlibcRand.h
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#ifndef GLIBCRAND_H_
#define GLIBCRAND_H_

#include <random>
#include "PRNG.h"

static const std::string GLIBC_RAND = "glibc-rand";

class GlibcRand: public PRNG
{
public:
    GlibcRand();
    virtual ~GlibcRand();

    const std::string getName(void);
    void seed(uint32_t value);
    uint32_t getSeed(void);
    uint32_t random(void);

private:
    uint32_t seedValue;
};

#endif /* GLIBCRAND_H_ */
