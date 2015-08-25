#ifndef PHP_MT19937_H_
#define PHP_MT19937_H_

#include <random>
#include "PRNG.h"

static const std::string PHP_MT_RAND = "php-mt_rand";
static const uint32_t PHP_MT_RAND_STATE_SIZE = 624;

#define hiBit(u)      ((u) & 0x80000000U)  /* mask all but highest   bit of u */
#define loBit(u)      ((u) & 0x00000001U)  /* mask all but lowest    bit of u */
#define loBits(u)     ((u) & 0x7FFFFFFFU)  /* mask     the highest   bit of u */
#define mixBits(u, v) (hiBit(u)|loBits(v)) /* move hi bit of u to hi bit of v */
#define php_twist(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(loBit(u))) & 0x9908b0dfU))

class PHP_mt19937: public PRNG
{
public:
    PHP_mt19937();
    virtual ~PHP_mt19937();

    const std::string getName(void);
    void seed(int64_t value);
    int64_t getSeed(void);
    uint32_t random(void);

    uint32_t getStateSize(void);
    void setState(std::vector<uint32_t>);
    std::vector<uint32_t> getState(void);

    void setEvidence(std::vector<uint32_t>);

    std::vector<uint32_t> predictForward(uint32_t);
    std::vector<uint32_t> predictBackward(uint32_t);
    void tune(std::vector<uint32_t>, std::vector<uint32_t>);

    bool reverseToSeed(int64_t *, uint32_t);
    void setBounds(uint32_t, uint32_t);

    int64_t getMinSeed();
    int64_t getMaxSeed();

private:
    void php_mt_initialize(uint32_t s);
    void php_mt_reload();
    uint32_t genrand_int32(struct MT *mt);

    MT *m_mt;
    uint32_t m_seedValue;

    /* rand.c */
    uint32_t m_state[N+1];  /* state vector + 1 extra to not violate ANSI C */
    uint32_t *m_next;          /* next random value is computed from here */
    int m_left;                /* can *next++ this many times before reloading */

};

#endif /* PHP_MT19937_H_ */
