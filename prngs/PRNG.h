/*
 * PRNG.h
 *
 *      This is a blank pure virtual interface class
 *      all the PRNGs implement this interface.
 */

#ifndef PRNG_H_
#define PRNG_H_

#include <vector>

/*
Common MT data structures and constants
*/
/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfU	/* constant vector a */
#define UMASK 0x80000000U	/* most significant w-r bits */
#define LMASK 0x7fffffffU	/* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS((u),(v)) >> 1) ^ ((v)&1U ? MATRIX_A : 0U))

enum {MT_MAX_STATE = N};

struct MT {
    /* assume int is enough to store 32bits */
    unsigned int state[N]; /* the array for the state vector  */
    unsigned int *next;
    int left;
};

class PRNG
{
public:

    virtual const std::string getName(void) = 0;
    virtual void seed(int64_t) = 0;
    virtual int64_t getSeed(void) = 0;
    virtual uint32_t random(void) = 0;
    virtual uint32_t getStateSize(void) = 0;
    virtual void setState(std::vector<uint32_t>) = 0;
    virtual std::vector<uint32_t> getState(void) = 0;
    virtual void setEvidence(std::vector<uint32_t>) = 0;
    virtual std::vector<uint32_t> predictForward(uint32_t) = 0;
    virtual std::vector<uint32_t> predictBackward(uint32_t) = 0;
    virtual void tune(std::vector<uint32_t>, std::vector<uint32_t>) = 0;
    virtual bool reverseToSeed(int64_t *, uint32_t) = 0;
    virtual void setBounds(uint32_t, uint32_t) = 0;
    virtual int64_t getMinSeed() = 0;
    virtual int64_t getMaxSeed() = 0;

    virtual ~PRNG(){};

protected:
    std::vector<uint32_t> m_state;
    std::vector<uint32_t> m_evidence;
    uint32_t m_minBound;
    uint32_t m_maxBound;
    bool m_isBounded;
};

#endif /* PRNG_H_ */
