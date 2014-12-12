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

#include "Untwister.h"


Untwister::Untwister()
{
    isCompleted = new std::atomic<bool>(false);
    isRunning = new std::atomic<bool>(false);
    observedOutputs = new std::vector<uint32_t>;
    depth = DEFAULT_DEPTH;
    minConfidence = DEFAULT_MIN_CONFIDENCE;
    threads = std::thread::hardware_concurrency();
    answers = new std::vector<std::vector<Seed>* >(threads);
    status = new std::vector<uint32_t>(threads);
}

Untwister::Untwister(unsigned int observationSize)
{
    isCompleted = new std::atomic<bool>(false);
    isRunning = new std::atomic<bool>(false);
    observedOutputs = new std::vector<uint32_t>(observationSize);
    depth = DEFAULT_DEPTH;
    minConfidence = DEFAULT_MIN_CONFIDENCE;
    threads = std::thread::hardware_concurrency();
    answers = new std::vector<std::vector<Seed>* >(threads);
    status = new std::vector<uint32_t>(threads);
}

Untwister::~Untwister()
{
    delete isCompleted;
    delete isRunning;
    delete observedOutputs;
    delete answers;
    delete status;
}

std::vector<Seed> Untwister::bruteforce(uint32_t lowerBoundSeed, uint32_t upperBoundSeed)
{

    std::vector<uint32_t> labor = divisionOfLabor(upperBoundSeed - lowerBoundSeed, threads);
    uint32_t startAt = lowerBoundSeed;

    std::vector<std::thread> pool = std::vector<std::thread>(threads);
    for(unsigned int id = 0; id < threads; ++id)
    {
        uint32_t endAt = startAt + labor.at(id);
        pool[id] = std::thread(&Untwister::worker, this, id, startAt, endAt);
        startAt += labor.at(id);
    }

    isRunning->store(true, std::memory_order_relaxed);

    for(unsigned int id = 0; id < pool.size(); ++id)
    {
        pool[id].join();
    }

    std::vector<Seed> results = std::vector<Seed>();
    for (unsigned int id = 0; id < answers->size(); ++id)
    {
        for (unsigned int index = 0; index < answers->at(id)->size(); ++index)
        {
            results.push_back(answers->at(id)->at(index));
        }
        delete answers->at(id);
    }

    return results;
}

/* This method is execute as a seperate thread */
void Untwister::worker(unsigned int id, uint32_t startingSeed, uint32_t endingSeed)
{

    /* Each thread must have a local factory unless you like mutexes and/or segfaults */
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(prng);
    answers->at(id) = new std::vector<Seed>();

    for(uint32_t seedIndex = startingSeed; seedIndex <= endingSeed; ++seedIndex)
    {

        if(isCompleted->load(std::memory_order_relaxed))
        {
            break;  // Some other thread found the seed
        }

        generator->seed(seedIndex);
        uint32_t matchesFound = 0;
        for(uint32_t index = 0; index < depth; index++)
        {
            uint32_t nextRand = generator->random();
            uint32_t observed = observedOutputs->at(matchesFound);

            if(observed == nextRand)
            {
                matchesFound++;
                if(matchesFound == observedOutputs->size())
                {
                    break;  // This seed is a winner if we get to the end
                }
            }
        }

        status->at(id) = seedIndex - startingSeed;
        double confidence = ((double) matchesFound / (double) observedOutputs->size()) * 100.0;
        if(minConfidence <= confidence)
        {
            Seed seed = Seed(seedIndex, confidence);
            answers->at(id)->push_back(seed);
        }
        if(matchesFound == observedOutputs->size())
        {
            isCompleted->store(true, std::memory_order_relaxed);
        }
    }
    delete generator;
}


/*
    This is the "smarter" method of breaking RNGs. We use consecutive integers
    to infer information about the internal state of the RNG. Using this
    method, however, we won't typically recover an actual seed value.
    But the effect is the same.
*/
bool Untwister::inferState()
{

    PRNGFactory factory;
    PRNG *generator = factory.getInstance(prng);
    uint32_t stateSize = generator->getStateSize();

    if(observedOutputs->size() <= stateSize)
    {
        std::cout << WARN << "Not enough observed values to perform state inference,"
                  << " try again with more than " << stateSize << " values." << std::endl;
        return false;
    }

    double highscore = 0.0;

    /* Guaranteed from the above to loop at least one time */
    std::vector<double> scores;
    std::vector<uint32_t> best_state;
    for(uint32_t index = 0; index < (observedOutputs->size() - stateSize); ++index)
    {
        std::vector<uint32_t>::const_iterator first = observedOutputs->begin() + index;
        std::vector<uint32_t>::const_iterator last = observedOutputs->begin() + index + stateSize;
        std::vector<uint32_t> state(first, last);

        /* Make predictions based on the state */
        std::vector<uint32_t> evidenceForward
            ((std::vector<uint32_t>::const_iterator)observedOutputs->begin(), first);
        std::vector<uint32_t> evidenceBackward
            (last+1, (std::vector<uint32_t>::const_iterator)observedOutputs->end());
        generator->setState(state);

        /* Provide additional evidence for tuning on PRNGs that require it */
        generator->setEvidence((*observedOutputs));
        generator->tune(evidenceForward, evidenceBackward);

        std::vector<uint32_t> predictions_forward =
            generator->predictForward(((observedOutputs->size() - stateSize) - index));

        std::vector<uint32_t> predictions_backward = generator->predictBackward(index);

        /* Test the prediction against the rest of the observed data */
        /* Forward */
        uint32_t matchesFound = 0;
        uint32_t index_pred = 0;
        uint32_t index_obs = index + stateSize;
        while(index_obs < observedOutputs->size() && index_pred < predictions_forward.size())
        {
            if(observedOutputs->at(index_obs) == predictions_forward[index_pred])
            {
                matchesFound++;
                index_obs++;
            }
            index_pred++;
        }

        /* Backward */
        index_pred = 0;
        index_obs = index;
        while(index_obs > 0 && index_pred < predictions_backward.size())
        {
            if(observedOutputs->at(index_obs) == predictions_backward[index_pred])
            {
                matchesFound++;
                index_obs--;
            }
            index_pred++;
        }

        /* If we get a perfect guess, then try reversing out the seed, and exit */
        if(matchesFound == (observedOutputs->size() - stateSize))
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

        double score = (double)(matchesFound*100) / (double)(observedOutputs->size() - stateSize);
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

    delete generator;
    return false;
}

std::vector<uint32_t> Untwister::generateSampleFromSeed(uint32_t seed)
{
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(prng);
    generator->seed(seed);
    PRNG *distance_gen = factory.getInstance(prng);
    distance_gen->seed(time(NULL));
    uint32_t distance = distance_gen->random() % (depth - 10);

    // Burn a bunch of random numbers
    for (uint32_t index = 0; index < distance; ++index)
    {
        generator->random();
    }

    std::vector<uint32_t> results(10);
    for (unsigned int index = 0; index < 10; ++index)
    {
        results.push_back(generator->random());
    }
    delete generator;
    delete distance_gen;
    return results;
}

std::vector<uint32_t> Untwister::generateSampleFromState()
{
    PRNGFactory factory;
    PRNG *generator = factory.getInstance(prng);
    generator->setState((*observedOutputs));

    std::vector<uint32_t> results(depth);
    for (unsigned int index = 0; index < depth; ++index)
    {
        results.push_back(generator->random());
    }
    delete generator;
    return results;
}

/* Divide X work among Y number of threads, and evenly distribute remainders */
std::vector<uint32_t> Untwister::divisionOfLabor(uint32_t sizeOfWork, uint32_t numberOfWorkers)
{
    uint32_t work = sizeOfWork / numberOfWorkers;
    uint32_t leftover = sizeOfWork % numberOfWorkers;
    std::vector<uint32_t> labor(numberOfWorkers);
    for(uint32_t index = 0; index < numberOfWorkers; ++index)
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

void Untwister::addObservedOutput(uint32_t observedOutput)
{
    observedOutputs->push_back(observedOutput);
}

std::vector<uint32_t>* Untwister::getObservedOutputs()
{
    return observedOutputs;
}

std::vector<uint32_t>* Untwister::getStatus()
{
    return status;
}

std::vector<std::string> Untwister::getPRNGNames()
{
    PRNGFactory factory;
    return factory.getNames();
}


void Untwister::setPRNG(std::string prng)
{
    if (isSupportedPRNG(prng))
    {
        this->prng = prng;
    }
}

void Untwister::setPRNG(char *prng)
{
    setPRNG(std::string(prng));
}

std::string Untwister::getPRNG()
{
    return prng;
}

bool Untwister::isSupportedPRNG(char *prng)
{
    return isSupportedPRNG(std::string(prng));
}

bool Untwister::isSupportedPRNG(std::string prng)
{
    std::vector<std::string> names = getPRNGNames();
    return std::find(names.begin(), names.end(), prng) == names.end() ? false:true;
}

void Untwister::setThreads(unsigned int threads)
{
    if (!isRunning->load(std::memory_order_relaxed))
    {
        this->threads = threads;
        delete answers;
        delete status;
        answers = new std::vector<std::vector<Seed>* >(threads);
        status = new std::vector<uint32_t>(threads);
    }
}

unsigned int Untwister::getThreads()
{
    return threads;
}

void Untwister::setDepth(unsigned int depth)
{
    this->depth = depth;
}

unsigned int Untwister::getDepth()
{
    return depth;
}

void Untwister::setMinConfidence(double minConfidence)
{
    this->minConfidence = minConfidence;
}

double Untwister::getMinConfidence()
{
    return minConfidence;
}

std::atomic<bool>* Untwister::getIsCompleted()
{
    return isCompleted;
}

std::atomic<bool>* Untwister::getIsRunning()
{
    return isRunning;
}
