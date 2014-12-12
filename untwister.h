/*
    Copyright Dan Petro, 2014

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UNTWISTER_H_
#define UNTWISTER_H_

#include <climits>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <iostream>

#include "ConsoleColors.h"
#include "PRNGFactory.h"
#include "prngs/PRNG.h"

// Pair of <seed, quality of fit>
typedef std::pair<uint32_t, double> Seed;

static const uint32_t DEFAULT_DEPTH = 1000;
static const double DEFAULT_MIN_CONFIDENCE = 100.0;

class Untwister
{

public:
    Untwister();
    Untwister(unsigned int threads);
    Untwister(unsigned int threads, unsigned int observationSize);
    virtual ~Untwister();

    std::vector<Seed> bruteforce(uint32_t lowerBoundSeed, uint32_t upperBoundSeed);
    bool inferState();

    std::vector<std::string> getPRNGNames();
    void setPRNGName(std::string prng);
    void setPRNGName(char *prng);
    std::string getPRNGName();

    void setMinConfidence(double minConfidence);
    double getMinConfidence();
    void setDepth(uint32_t depth);
    uint32_t getDepth();
    void setThreads(unsigned int threads);
    unsigned int getThreads();
    void addObservedOutput(uint32_t observedOutput);
    std::vector<uint32_t>* getObservedOutputs();
    std::vector<uint32_t>* getStatus();
    std::atomic<bool>* getIsCompleted();
    std::atomic<bool>* getIsRunning();

    std::vector<uint32_t> generateSampleFromSeed(uint32_t seed);
    std::vector<uint32_t> generateSampleFromState();

private:
    unsigned int threads;
    double minConfidence;
    uint32_t depth;
    std::string prng;
    std::atomic<bool> *isCompleted;
    std::atomic<bool> *isRunning;
    std::vector<uint32_t> *status;
    std::vector<std::vector<Seed>* > *answers;
    std::vector<uint32_t> *observedOutputs;

    void worker(unsigned int id, uint32_t startingSeed, uint32_t endingSeed);
    std::vector<uint32_t> divisionOfLabor(uint32_t sizeOfWork, uint32_t numberOfWorkers);

};

#endif /* UNTWISTER_H_ */
