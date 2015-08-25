#ifndef MT19937_H_
#define MT19937_H_

#include <random>
#include "PRNG.h"

static const std::string MT19937 = "mt19937";
static const uint32_t MT19937_STATE_SIZE = 624;

class Mt19937: public PRNG
{
public:
    Mt19937();
    virtual ~Mt19937();

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
    uint32_t seedValue;
    std::mt19937 generator;
};

#endif /* MT19937_H_ */
