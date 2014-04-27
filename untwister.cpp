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


#include <getopt.h>

#include "untwister.h"
#include "ConsoleColors.h"


void Usage(PRNGFactory factory, unsigned int threads)
{
    std::cout << BOLD << "Untwister" << RESET << " - Recover PRNG seeds from observed values." << std::endl;
    std::cout << "\t-i <input_file> [-d <depth> ] [-r <prng>] [-g <seed>] [-t <threads>] [-c <confidence>]\n" << std::endl;
    std::cout << "\t-i <input_file>\n\t\tPath to file input file containing observed results of your RNG. The contents" << std::endl;
    std::cout << "\t\tare expected to be newline separated 32-bit integers. See test_input.txt for" << std::endl;
    std::cout << "\t\tan example." << std::endl;
    std::cout << "\t-d <depth>\n\t\tThe depth (default 1000) to inspect for each seed value when brute forcing." << std::endl;
    std::cout << "\t\tChoosing a higher depth value will make brute forcing take longer (linearly), but is" << std::endl;
    std::cout << "\t\trequired for cases where the generator has been used many times already." << std::endl;
    std::cout << "\t-r <prng>\n\t\tThe PRNG algorithm to use. Supported PRNG algorithms:" << std::endl;
    std::vector<std::string> names = factory.getNames();
    for (unsigned int index = 0; index < names.size(); ++index)
    {
        std::cout << "\t\t" << BOLD << " * " << RESET << names[index];
        if (index == 0)
            std::cout << " (default)";
        std::cout << std::endl;
    }
    std::cout << "\t-u\n\t\tUse bruteforce, but only for unix timestamp values within a range of +/- 1 " << std::endl;
    std::cout << "\t\tyear from the current time." << std::endl;
    std::cout << "\t-g <seed>\n\t\tGenerate a test set of random numbers from the given seed (at a random depth)" << std::endl;
    std::cout << "\t-c <confidence>\n\t\tSet the minimum confidence percentage to report" << std::endl;
    std::cout << "\t-t <threads>\n\t\tSpawn this many threads (default is " << threads << ")" << std::endl;
    std::cout << "" << std::endl;
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

void SpawnThreads(const unsigned int threads, std::vector<std::vector<Seed>* > *answers, double minimumConfidence,
        uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth, std::string rng)
{
    bool isCompleted = false;  // Flag to tell threads to stop working

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

void FindSeed(const std::string& rng, unsigned int threads, double minimumConfidence, uint32_t lowerBoundSeed,
        uint32_t upperBoundSeed, uint32_t depth)
{
    std::cout << INFO << "Looking for seed using " << rng << std::endl;
    std::cout << INFO << "Spawning " << threads << " worker thread(s) ..." << std::endl;

    /* Each thread needs their own set of answers to avoid locking */
    std::vector<std::vector<Seed>* > *answers = new std::vector<std::vector<Seed>* >(threads);
    steady_clock::time_point elapsed = steady_clock::now();
    SpawnThreads(threads, answers, minimumConfidence, lowerBoundSeed, upperBoundSeed, depth, rng);

    std::cout << INFO << "Completed in " << duration_cast<seconds>(steady_clock::now() - elapsed).count()
              << " second(s)" << std::endl;

    /* Display results */
    for (unsigned int id = 0; id < answers->size(); ++id)
    {
        /* Look for answers from each thread */
        for (unsigned int index = 0; index < answers->at(id)->size(); ++index)
        {
            std::cout << SUCCESS << "Found seed " << answers->at(id)->at(index).first
                      << " with a confidence of " << answers->at(id)->at(index).second
                      << '%' << std::endl;
        }
        delete answers->at(id);
    }
    delete answers;
}

int main(int argc, char *argv[])
{
    int c;
    unsigned int threads = std::thread::hardware_concurrency();
    uint32_t lowerBoundSeed = 0;
    uint32_t upperBoundSeed = UINT_MAX;
    uint32_t depth = 1000;
    uint32_t seed = 0;
    double minimumConfidence = 100.0;
    PRNGFactory factory;
    std::string rng = factory.getNames()[0];

    while ((c = getopt(argc, argv, "d:i:g:t:r:c:uh")) != -1)
    {
        switch (c)
        {
            case 'g':
            {
                seed = strtoul(optarg, NULL, 10);
                break;
            }
            case 'u':
            {
                lowerBoundSeed = time(NULL) - ONE_YEAR;
                upperBoundSeed = time(NULL) + ONE_YEAR;
                break;
            }
            case 'r':
            {
                rng = optarg;
                std::vector<std::string> names = factory.getNames();
                if (std::find(names.begin(), names.end(), rng) == names.end())
                {
                    std::cerr << WARN << "ERROR: The PRNG \"" << optarg << "\" is not supported, see -h" << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'd':
            {
                depth = strtoul(optarg, NULL, 10);
                if (depth == 0)
                {
                    std::cerr << WARN << "ERROR: Please enter a valid depth > 1" << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'i':
            {
                std::ifstream infile(optarg);
                if (!infile)
                {
                    std::cerr << WARN << "ERROR: File \"" << optarg << "\" not found" << std::endl;
                }
                std::string line;
                while (std::getline(infile, line))
                {
                    observedOutputs.push_back(strtoul(line.c_str(), NULL, 10));
                }
                break;
            }
            case 't':
            {
                threads = strtoul(optarg, NULL, 10);
                if (threads == 0)
                {
                    std::cerr << WARN << "ERROR: Please enter a valid number of threads > 1" << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'c':
            {
                minimumConfidence = ::atof(optarg);
                if (minimumConfidence <= 0 || 100.0 < minimumConfidence)
                {
                    std::cerr << WARN << "ERROR: Invalid confidence percentage " << std::endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'h':
            {
                Usage(factory, threads);
                return EXIT_SUCCESS;
            }
            case '?':
            {
                if (optopt == 'd')
                   std::cerr << "Option -" << optopt << " requires an argument." << std::endl;
                else if (isprint(optopt))
                   std::cerr << "Unknown option `-" << optopt << "'." << std::endl;
                else
                   std::cerr << "Unknown option character `" << optopt << "'." << std::endl;
                Usage(factory, threads);
                return EXIT_FAILURE;
            }
            default:
            {
                Usage(factory, threads);
                return EXIT_FAILURE;
            }
        }
    }

    if (seed != 0)
    {
        GenerateSample(seed, depth, rng);
        return EXIT_SUCCESS;
    }

    if (observedOutputs.empty())
    {
        Usage(factory, threads);
        std::cerr << WARN << "ERROR: No input numbers provided. Use -i <file> to provide a file" << std::endl;
        return EXIT_FAILURE;
    }

    FindSeed(rng, threads, minimumConfidence, lowerBoundSeed, upperBoundSeed, depth);
    return EXIT_SUCCESS;
}

