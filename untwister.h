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

#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include "ConsoleColors.h" // TODO: Sepearte logic from stdout calls
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
void BruteForce(unsigned int id, std::atomic<bool> *isCompleted, std::vector<std::vector<Seed>* > *answers,
        std::vector<uint32_t> *status, double minimumConfidence, uint32_t startingSeed, uint32_t endingSeed,
        uint32_t depth, std::string rng)
{
    /* Each thread must have a local factory unless you like mutexes and/or segfaults */
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(rng);
    answers->at(id) = new std::vector<Seed>;

    for(uint32_t seedIndex = startingSeed; seedIndex <= endingSeed; ++seedIndex)
    {

        if(isCompleted->load(std::memory_order_relaxed))
            break;  // Some other thread found the seed

        generator->seed(seedIndex);
        uint32_t matchesFound = 0;
        for(uint32_t index = 0; index < depth; index++)
        {
            uint32_t nextRand = generator->random();
            uint32_t observed = observedOutputs[matchesFound];

            if(observed == nextRand)
            {
                matchesFound++;
                if(matchesFound == observedOutputs.size())
                {
                    break;  // This seed is a winner if we get to the end
                }
            }
        }

        status->at(id) = seedIndex - startingSeed;
        double confidence = ((double) matchesFound / (double) observedOutputs.size()) * 100.0;
        if(minimumConfidence <= confidence)
        {
            Seed seed = Seed(seedIndex, confidence);
            answers->at(id)->push_back(seed);
        }
        if(matchesFound == observedOutputs.size())
        {
            isCompleted->store(true, std::memory_order_relaxed);
        }
    }
    delete generator;
}

/* Divide X number of seeds among Y number of threads */
std::vector<uint32_t> DivisionOfLabor(uint32_t sizeOfWork, uint32_t numberOfWorkers)
{
    uint32_t work = sizeOfWork / numberOfWorkers;
    uint32_t leftover = sizeOfWork % numberOfWorkers;
    std::vector<uint32_t> labor(numberOfWorkers);
    for(uint32_t index = 0; index < numberOfWorkers; ++index)
    {
        if(0 < leftover)
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

/* Generic Threading */
void StartBruteForce(unsigned int threads, std::vector<std::vector<Seed>* >* answers, double minimumConfidence,
        uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth, std::string rng)
{
    std::atomic<bool> *isCompleted = new std::atomic<bool>(false);
    std::vector<std::thread> pool(threads);
    std::vector<uint32_t> *status = new std::vector<uint32_t>(threads);
    std::vector<uint32_t> labor = DivisionOfLabor(upperBoundSeed - lowerBoundSeed, threads);
    uint32_t startAt = lowerBoundSeed;

    for(unsigned int id = 0; id < threads; ++id)
    {
        uint32_t endAt = startAt + labor.at(id);
        pool[id] = std::thread(BruteForce, id, isCompleted, answers, status, minimumConfidence, startAt, endAt, depth, rng);
        startAt += labor.at(id);
    }

    for(unsigned int id = 0; id < pool.size(); ++id)
    {
        pool[id].join();
    }
    delete status;
}


/*
    This is the "smarter" method of breaking RNGs. We use consecutive integers
    to infer information about the internal state of the RNG. Using this
    method, however, we won't typically recover an actual seed value.
    But the effect is the same.
*/
bool InferState(const std::string& rng)
{
    std::cout << INFO << "Trying state inference" << std::endl;

    PRNGFactory factory;
    PRNG *generator = factory.getInstance(rng);
    uint32_t stateSize = generator->getStateSize();

    if(observedOutputs.size() <= stateSize)
    {
        std::cout << WARN << "Not enough observed values to perform state inference." << std::endl;
        std::cout << WARN << "Try again with more than " << stateSize << " values" << std::endl;
        return false;
    }

    double highscore = 0.0;

    /* Guaranteed from the above to loop at least one time */
    std::vector<double> scores;
    std::vector<uint32_t> best_state;
    for(uint32_t i = 0; i < (observedOutputs.size() - stateSize); i++)
    {
        std::vector<uint32_t>::const_iterator first = observedOutputs.begin() + i;
        std::vector<uint32_t>::const_iterator last = observedOutputs.begin() + i + stateSize;
        std::vector<uint32_t> state(first, last);

        /* Make predictions based on the state */
        std::vector<uint32_t> evidenceForward
            ((std::vector<uint32_t>::const_iterator)observedOutputs.begin(), first);
        std::vector<uint32_t> evidenceBackward
            (last+1, (std::vector<uint32_t>::const_iterator)observedOutputs.end());
        generator->setState(state);

        /* Provide additional evidence for tuning on PRNGs that require it */
        generator->setEvidence(observedOutputs);
        generator->tune(evidenceForward, evidenceBackward);

        std::vector<uint32_t> predictions_forward =
            generator->predictForward(((observedOutputs.size() - stateSize) - i));
        std::vector<uint32_t> predictions_backward =
            generator->predictBackward(i);

        /* Test the prediction against the rest of the observed data */
        /* Forward */
        uint32_t matchesFound = 0;
        uint32_t index_pred = 0;
        uint32_t index_obs = i + stateSize;
        while(index_obs < observedOutputs.size() && index_pred < predictions_forward.size())
        {
            if(observedOutputs[index_obs] == predictions_forward[index_pred])
            {
                matchesFound++;
                index_obs++;
            }
            index_pred++;
        }

        /* Backward */
        index_pred = 0;
        index_obs = i;
        while(index_obs > 0 && index_pred < predictions_backward.size())
        {
            if(observedOutputs[index_obs] == predictions_backward[index_pred])
            {
                matchesFound++;
                index_obs--;
            }
            index_pred++;
        }

        /* If we get a perfect guess, then try reversing out the seed, and exit */
        if(matchesFound == (observedOutputs.size() - stateSize))
        {
            uint32_t outSeed = 0;
            if(generator->reverseToSeed(&outSeed, 10000))
            {
                /* We win! */
                std::cout << SUCCESS << "Found seed " << outSeed << std::endl;
            }
            else
            {
                std::cout << SUCCESS << "Found state: " << std::endl;
                std::vector<uint32_t> state = generator->getState();
                for(uint32_t j = 0; j < state.size(); j++)
                {
                    std::cout << state[j] << std::endl;
                }
            }
            return true;
        }

        double score = (double)(matchesFound*100) / (double)(observedOutputs.size() - stateSize);
        scores.push_back(score);
        if(score > highscore)
        {
            best_state = generator->getState();
        }
    }

    /* Analyze scores */
    //TODO
    if(highscore > 0)
    {
        std::cout << SUCCESS << "Best state guess, with confidence of: " << highscore << '%' << std::endl;
        std::vector<uint32_t> state = generator->getState();
        for(uint32_t j = 0; j < state.size(); j++)
        {
            std::cout << SUCCESS << state[j] << std::endl;
        }
    }
    else
    {
        std::cout << INFO << "State Inference failed" << std::endl;
    }

    return false;
}

/* For easier testing, will generate a series of random numbers at a given seed and depth*/
std::vector<uint32_t> GenerateSample(uint32_t seed, uint32_t depth, std::string rng)
{
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(rng);
    generator->seed(seed);

    std::vector<uint32_t> results;
    for (unsigned int index = 0; index < depth; ++index)
    {
        results.push_back(generator->random());
    }
    delete generator;
    return results;
}

/* For easier testing, will generate a series of random numbers at a given state and depth */
std::vector<uint32_t> GenerateSample(std::vector<uint32_t> state, uint32_t depth, std::string rng)
{
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(rng);
    generator->setState(state);

    std::vector<uint32_t> results;
    for (unsigned int index = 0; index < depth; ++index)
    {
        results.push_back(generator->random());
    }
    delete generator;
    return results;
}

#endif /* UNTWISTER_H_ */
