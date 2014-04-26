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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>

#include "ConsoleColors.h"
#include "PRNGFactory.h"
#include "prngs/PRNG.h"

using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

// Pair of <seed, quality of fit>
typedef std::pair<uint32_t, double> Seed;

static std::vector<uint32_t> observedOutputs;
static const unsigned int ONE_YEAR = 31536000;


/* Yeah lots of parameters, but such is the life of a thread */
void BruteForce(const unsigned int id, bool& isCompleted, std::vector<std::vector<Seed>* > *answers,
        std::vector<uint32_t>* status, double minimumConfidence, uint32_t startingSeed, uint32_t endingSeed,
        uint32_t depth, std::string rng)
{
    /* Each thread must have a local factory unless you like mutexes and/or segfaults */
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(rng);
    answers->at(id) = new std::vector<Seed>;

    for (uint32_t seedIndex = startingSeed; seedIndex <= endingSeed; ++seedIndex)
    {
        generator->seed(seedIndex);

        uint32_t matchesFound = 0;
        for (uint32_t index = 0; index < depth; index++)
        {
            uint32_t nextRand = generator->random();
            uint32_t observed = observedOutputs[matchesFound];

            if (observed == nextRand)
            {
                matchesFound++;
                if (matchesFound == observedOutputs.size())
                {
                    break;  // This seed is a winner if we get to the end
                }
            }
        }

        if (isCompleted)
        {
            break;  // Some other thread found the seed
        }

        status->at(id) = seedIndex - startingSeed;
        double confidence = ((double) matchesFound / (double) observedOutputs.size()) * 100.0;
        if (minimumConfidence <= confidence)
        {
            Seed seed = {seedIndex, confidence};
            answers->at(id)->push_back(seed);
        }
        if (matchesFound == observedOutputs.size())
            isCompleted = true;  // We found the correct seed
    }
    delete generator;
}

void GenerateSample(uint32_t seed, uint32_t depth, std::string rng)
{
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(rng);
    generator->seed(seed);
    PRNG *distance_gen= factory.getInstance(rng);
    distance_gen->seed(time(NULL));
    uint32_t distance = distance_gen->random() % (depth - 10);

    // Burn a bunch of random numbers
    for (uint32_t index = 0; index < distance; ++index)
    {
        generator->random();
    }

    for (unsigned int index = 0; index < 10; ++index)
    {
        std::cout << generator->random() << std::endl;
    }
    delete generator;
    delete distance_gen;
}

void StatusThread(std::vector<std::thread>& pool, bool& isCompleted, uint32_t totalWork, std::vector<uint32_t> *status)
{
    double percent = 0;
    steady_clock::time_point start = steady_clock::now();
    while (!isCompleted)
    {
        unsigned int sum = 0;
        for (unsigned int index = 0; index < status->size(); ++index)
        {
            sum += status->at(index);
        }
        percent = ((double) sum / (double) totalWork) * 100.0;
        isCompleted = (100.0 <= percent);
        std::cout << CLEAR << DEBUG << "Progress: " << percent << '%';
        std::cout << " (" << (int) duration_cast<seconds>(steady_clock::now() - start).count() << " seconds)";
        std::cout.flush();
        std::this_thread::sleep_for(milliseconds(150));
    }
    std::cout << CLEAR;
}

/* Divide X number of seeds among Y number of threads */
std::vector<uint32_t> DivisionOfLabor(uint32_t sizeOfWork, uint32_t numberOfWorkers)
{
    uint32_t work = sizeOfWork / numberOfWorkers;
    uint32_t leftover = sizeOfWork % numberOfWorkers;
    std::vector<uint32_t> labor(numberOfWorkers);
    for (uint32_t index = 0; index < numberOfWorkers; ++index)
    {
        if (0 < leftover)
        {
            labor[index] = work + 1;
            --leftover;
        }
        else
        {
            labor[index] = work;
        }
    }
    return labor;
}

void SpawnThreads(const unsigned int threads, std::vector<std::vector<Seed>* > *answers, double minimumConfidence,
        uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth, std::string rng)
{
    bool isCompleted = false;  // Flag to tell threads to stop working
    std::cout << INFO << "Spawning " << threads << " worker thread(s) ..." << std::endl;

    std::vector<std::thread> pool(threads);
    std::vector<uint32_t> *status = new std::vector<uint32_t>(threads);
    std::vector<uint32_t> labor = DivisionOfLabor(upperBoundSeed - lowerBoundSeed, threads);
    uint32_t startAt = lowerBoundSeed;
    for (unsigned int id = 0; id < threads; ++id)
    {
        uint32_t endAt = startAt + labor.at(id);
        pool[id] = std::thread(BruteForce, id, std::ref(isCompleted), answers, status, minimumConfidence, startAt, endAt, depth, rng);
        startAt += labor.at(id);
    }
    StatusThread(pool, isCompleted, upperBoundSeed - lowerBoundSeed, status);
    for (unsigned int id = 0; id < pool.size(); ++id)
    {
        pool[id].join();
    }

    delete status;
}

#endif /* UNTWISTER_H_ */
