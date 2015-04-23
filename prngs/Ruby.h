/*
 * Ruby.h
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#ifndef RUBY_H_
#define RUBY_H_

#include <string>
#include "PRNG.h"

static const std::string RUBY_RAND = "ruby-rand";
static const uint32_t RUBY_STATE_SIZE = 624;

class Ruby: public PRNG
{
public:
    Ruby();
    virtual ~Ruby();

    const std::string getName(void);
    void seed(uint32_t value);
    uint32_t getSeed(void);
    uint32_t random(void);

    uint32_t getStateSize(void);
    void setState(std::vector<uint32_t>);
    std::vector<uint32_t> getState(void);

    void setEvidence(std::vector<uint32_t>);

    std::vector<uint32_t> predictForward(uint32_t);
    std::vector<uint32_t> predictBackward(uint32_t);
    void tune(std::vector<uint32_t>, std::vector<uint32_t>);

    bool reverseToSeed(uint32_t *, uint32_t);

private:
    void init_genrand(struct MT *mt, unsigned int s);
    void next_state(struct MT *mt);
    uint32_t genrand_int32(struct MT *mt);

    MT *m_mt;
    uint32_t seedValue;
};

#endif /* RUBY_H_ */
