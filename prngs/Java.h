#ifndef JAVA_INT_H_
#define JAVA_INT_H_

#include <string>
#include "PRNG.h"

static const std::string JAVA = "java";
static const uint32_t JAVA_STATE_SIZE = 1;

class Java: public PRNG
{
public:
    Java();
    virtual ~Java();

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
    int32_t next(int32_t bits);
    int64_t m_seedValue;
    int64_t m_originalSeed;
};

#endif /* JAVA_INT_H_ */
