#ifndef UNTWISTER_H_
#define UNTWISTER_H_

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <climits>
#include <cstdlib>
#include <cstdint>
#include <exception>
#include "ConsoleColors.h"
#include "prngs/PRNGFactory.h"
#include "prngs/PRNG.h"

// Pair of <seed, quality of fit>
typedef std::pair<int64_t, double> Seed;
typedef std::pair<std::vector<uint32_t>, double> State;

static const uint32_t DEFAULT_DEPTH = 1000;
static const double DEFAULT_MIN_CONFIDENCE = 100.0;

class Untwister
{

public:
    Untwister();
    Untwister(unsigned int observationSize);
    virtual ~Untwister();

    std::vector<Seed> bruteforce(int64_t lowerBoundSeed, int64_t upperBoundSeed);

    bool canInferState();
    State inferState();
    uint32_t getStateSize();

    /* Gets the min and max possible seed, for the given PRNG type */
    int64_t getMinSeed();
    int64_t getMaxSeed();

    std::vector<std::string> getSupportedPRNGs();
    void setPRNG(std::string prng);
    void setPRNG(char *prng);
    std::string getPRNG();
    bool isSupportedPRNG(std::string prng);
    bool isSupportedPRNG(char* prng);

    void setMinConfidence(double minConfidence);
    double getMinConfidence();
    void setDepth(uint32_t depth);
    uint32_t getDepth();
    void setThreads(unsigned int threads);
    unsigned int getThreads();
    void addObservedOutput(uint32_t observedOutput);
    std::vector<uint32_t>* getObservedOutputs();

    /* Returns NULL if there is no status to get. Such as if the bruteforce thread hasn't started */
    std::vector<uint32_t>* getStatus();
    std::atomic<bool>* getIsCompleted();
    std::atomic<bool>* getIsRunning();
    std::atomic<bool>* getIsStarting();

    void setBounds(uint32_t, uint32_t);
    bool isBounded();

void generateSampleFromSeed(uint32_t depth, int64_t seed);

private:
    unsigned int m_threads;
    double m_minConfidence;
    uint32_t m_depth;
    std::string m_prng;
    std::atomic<bool> *m_isStarting;
    std::atomic<bool> *m_isRunning;
    std::atomic<bool> *m_isCompleted;
    std::vector<uint32_t> *m_status;
    std::vector<std::vector<Seed>* > *m_answers;
    std::vector<uint32_t> *m_observedOutputs;

    std::atomic<bool> *m_isBounded;
    uint32_t m_minBound;
    uint32_t m_maxBound;

    void m_worker(unsigned int id, uint32_t startingSeed, uint32_t endingSeed);
    std::vector<uint64_t> m_divisionOfLabor(uint64_t sizeOfWork);

};

#endif /* UNTWISTER_H_ */
