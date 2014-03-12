/*
 * PRNG.h
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 *
 *      This is a blank pure virtual interface class
 *      all the PRNGs implement this interface.
 */

#ifndef PRNG_H_
#define PRNG_H_

class PRNG
{
public:

    virtual const std::string getName(void) = 0;
    virtual void seed(uint32_t) = 0;
    virtual uint32_t getSeed(void) = 0;
    virtual uint32_t random(void) = 0;
    virtual uint32_t getStateSize(void) = 0;
    virtual void setState(std::vector<uint32_t>) = 0;
    virtual std::vector<uint32_t> getState(void) = 0;

    virtual ~PRNG(){};

protected:
    uint32_t m_seedValue;
    std::vector<uint32_t> m_state;

};

#endif /* PRNG_H_ */
