/*
 * Mt19937.h
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#ifndef MT19937_H_
#define MT19937_H_

#include <random>
#include "PRNG.h"

static const std::string MT19937 = "mt19937";

class Mt19937: public PRNG
{
public:
    Mt19937();
    virtual ~Mt19937();

    const std::string getName(void);
    void seed(uint32_t value);
    uint32_t getSeed(void);
    uint32_t random(void);

private:
    uint32_t seedValue;
    std::mt19937 generator;
};

#endif /* MT19937_H_ */
