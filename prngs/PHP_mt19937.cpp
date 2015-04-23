#include "PHP_mt19937.h"

PHP_mt19937::PHP_mt19937()
{
    m_seedValue = 0;
    m_mt = NULL;
}

PHP_mt19937::~PHP_mt19937()
{
    delete m_mt;
    m_mt = NULL;
}

const std::string PHP_mt19937::getName()
{
    return PHP_MT_RAND;
}

void PHP_mt19937::seed(uint32_t value)
{
    delete m_mt;
    m_mt = new MT;
    m_seedValue = value;
    php_mt_initialize(m_seedValue);
    php_mt_reload();
}

uint32_t PHP_mt19937::getSeed()
{
    return m_seedValue;
}

uint32_t PHP_mt19937::random()
{
    return genrand_int32(m_mt) >> 1;
}

void PHP_mt19937::php_mt_initialize(uint32_t seed)
{
    register uint32_t *s = m_state;
    register uint32_t *r = m_state;
    register int i = 1;

    *s++ = seed & 0xffffffffU;
    for( ; i < N; ++i ) {
        *s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
        r++;
    }
}

void PHP_mt19937::php_mt_reload()
{
    register uint32_t *state = m_state;
    register uint32_t *p = state;
    register int i;

    for (i = N - M; i--; ++p)
        *p = php_twist(p[M], p[0], p[1]);
    for (i = M; --i; ++p)
        *p = php_twist(p[M-N], p[0], p[1]);
    *p = php_twist(p[M-N], p[0], state[0]);
    m_left = N;
    m_next = state;
}

uint32_t PHP_mt19937::genrand_int32(struct MT *mt)
{
    /* Pull a 32-bit integer from the generator state
       Every other access function simply transforms the numbers extracted here */

    register uint32_t s1;

    if (m_left == 0) {
        php_mt_reload();
    }
    --m_left;

    s1 = *m_next++;
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9d2c5680U;
    s1 ^= (s1 << 15) & 0xefc60000U;
    return ( s1 ^ (s1 >> 18) );
}

uint32_t PHP_mt19937::getStateSize(void)
{
    return PHP_MT_RAND_STATE_SIZE;
}

void PHP_mt19937::setState(std::vector<uint32_t> inState)
{
    std::copy(inState.begin(), inState.end(), m_state);
}

std::vector<uint32_t> PHP_mt19937::getState(void)
{
    std::vector<uint32_t> out(std::begin(m_state), std::end(m_state));
    return out;
}

std::vector<uint32_t> PHP_mt19937::predictForward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

std::vector<uint32_t> PHP_mt19937::predictBackward(uint32_t)
{
    std::vector<uint32_t> ret;
    //TODO
    return ret;
}

bool PHP_mt19937::reverseToSeed(uint32_t *outSeed, uint32_t depth)
{
    return false;
}

void PHP_mt19937::tune(std::vector<uint32_t> evidenceForward, std::vector<uint32_t> evidenceBackward)
{
    //TODO
}

void PHP_mt19937::setEvidence(std::vector<uint32_t>)
{

}
