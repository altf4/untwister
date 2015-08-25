/*
    Copyright Bishop Fox, 2014

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

#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <chrono>
#include <atomic>
#include <thread>
#include <fstream>
#include <iostream>

#include "ConsoleColors.h"
#include "Untwister.h"

using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

static const unsigned int ONE_YEAR = 31536000;


void Usage(Untwister *untwister)
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
    std::vector<std::string> names = untwister->getSupportedPRNGs();
    for (unsigned int index = 0; index < names.size(); ++index)
    {
        std::cout << "\t\t" << BOLD << " * " << RESET << names[index];
        if (index == 0)
        {
            std::cout << " (default)";
        }
        std::cout << std::endl;
    }
    std::cout << "\t-u\n\t\tUse bruteforce, but only for unix timestamp values within a range of +/- 1 " << std::endl;
    std::cout << "\t\tyear from the current time." << std::endl;
    std::cout << "\t-b\n\t\tAlways bruteforce, even if state inference attack is successful" << std::endl;
    std::cout << "\t-g <seed>\n\t\tGenerate a test set of random numbers from the given seed"  << std::endl;
    std::cout << "\t-D <depth>\n\t\tThe quantity of random numbers to generate when using the -g flag (default 20)" << std::endl;
    std::cout << "\t-c <confidence>\n\t\tSet the minimum confidence percentage to report" << std::endl;
    std::cout << "\t-t <threads>\n\t\tSpawn this many threads (default is " << untwister->getThreads() << ")" << std::endl;
    std::cout << "\t-m <min bound>\n\t\tSet the minimum bound (inclusive), for a bounded PRNG function" << std::endl;
    std::cout << "\t-M <max bound>\n\t\tSet the maximum bound (inclusive), for a bounded PRNG function" << std::endl;
    std::cout << "\t-s <min seed>\n\t\tSet the minimum seed (inclusive), for brute forcing (a 64 bit signed integer)" << std::endl;
    std::cout << "\t-S <max seed>\n\t\tSet the maxmimum seed (inclusive), for brute forcing (a 64 bit signed integer)" << std::endl;
    std::cout << std::endl;
}

void DisplayProgress(Untwister *untwister, uint64_t totalWork)
{
    std::atomic<bool>* isStarting = untwister->getIsStarting();
    while (isStarting->load(std::memory_order_relaxed))
    {
        std::this_thread::sleep_for(milliseconds(10));
    }

    std::cout.precision(2);
    double percent = 0.0;
    double seedsPerSec = 0.0;
    uint32_t timeLeft = 0.0;
    int secondsLeft = 0;
    int minutesLeft = 0;
    int hoursLeft = 0;
    int daysLeft = 0;
    int yearsLeft = 0;
    steady_clock::time_point started = steady_clock::now();
    std::vector<uint32_t> *status = untwister->getStatus();
    if(status == NULL)
    {
        /* If the status is NULL, we must be in a finished state, since we can't be starting (we already checked that)*/
        /*    and we can't be running. So, just clean up and quit if we get this.*/
        return;
    }
    std::atomic<bool> *isCompleted = untwister->getIsCompleted();
    char spinner[] = {'|', '/', '-', '\\'};
    unsigned int count = 0;
    while (!isCompleted->load(std::memory_order_relaxed))
    {
        unsigned int sum = 0;
        duration<double> time_span = duration_cast<duration<double>>(steady_clock::now() - started);
        for (unsigned int index = 0; index < status->size(); ++index)
        {
            sum += status->at(index);
        }
        percent = ((double) sum / (double) totalWork) * 100.0;
        if (0 < time_span.count())
        {
            seedsPerSec = (double) sum / (double) time_span.count();
            if (0 == count % 20)
            {
                timeLeft = ((totalWork - sum) / seedsPerSec);
                secondsLeft = timeLeft % 60;
                minutesLeft = (timeLeft / 60) % 60;
                hoursLeft = (timeLeft / 3600) % 24;
                daysLeft = (timeLeft / 86400) % 365;
                yearsLeft = (timeLeft / ONE_YEAR);
            }
        }

        std::cout << CLEAR << BOLD << PURPLE << "[" << spinner[count % 4] << "]" << RESET
                  << " Progress: " << std::fixed<< percent << '%'
                  << "  [" << sum << " / " << totalWork << "]"
                  << "  ~" << seedsPerSec << "/sec ";
        if(yearsLeft > 0)
        {
            std::cout << " " << yearsLeft << " years";
        }
        if(daysLeft > 0)
        {
            std::cout << " " << daysLeft << " days";
        }
        if(hoursLeft > 0)
        {
            std::cout << " " << hoursLeft << " hours";
        }
        if(minutesLeft > 0)
        {
            std::cout << " " << minutesLeft << " minutes";
        }
        std::cout << " " << secondsLeft << " seconds";
        std::cout.flush();
        ++count;
        std::this_thread::sleep_for(milliseconds(100));
    }
    std::cout << CLEAR;
}

bool inferenceAttack(Untwister *untwister)
{
    if (untwister->canInferState())
    {
        std::cout << INFO << "Attempting state inference attack" << std::endl;
        auto state = untwister->inferState();
        if (0 < state.second)
        {
            std::cout << "Recovered state: " << std::endl;
            for (uint32_t index = 0; index < state.first.size(); ++index)
            {
                std::cout << '\t' << state.first[index] << std::endl;
            }
            std::cout << SUCCESS << "Confidence: " << state.second << std::endl;
        }
        return 0 < state.second ? true:false;
    }
    else
    {
        std::cout << WARN << "Not enough observed values to perform state inference, "
                  << "try again with more than " << untwister->getStateSize() << " values." << std::endl;
        return false;
    }
}

void bruteforceAttack(Untwister *untwister, int64_t lowerBoundSeed, int64_t upperBoundSeed)
{
    std::cout << INFO << "Looking for seed using " << BOLD << untwister->getPRNG() << RESET << std::endl;
    std::cout << INFO << "Spawning " << untwister->getThreads() << " worker thread(s) ..." << std::endl;

    steady_clock::time_point elapsed = steady_clock::now();
    std::thread progressThread(DisplayProgress, untwister, upperBoundSeed - lowerBoundSeed);

    auto results = untwister->bruteforce(lowerBoundSeed, upperBoundSeed);

    progressThread.join();

    /* Total time elapsed */
    std::cout << INFO << "Completed in "
              << duration_cast<seconds>(steady_clock::now() - elapsed).count()
              << " second(s)" << std::endl;

    /* Display results */
    for (unsigned int index = 0; index < results.size(); ++index)
    {
        std::cout << SUCCESS << "Found seed " << results[index].first
                  << " with a confidence of " << results[index].second
                  << '%' << std::endl;
    }
}

int main(int argc, char *argv[])
{
    int c;

    int64_t lowerBoundSeed = 0;
    int64_t upperBoundSeed = UINT_MAX;
    bool manualSeedMinFlag = false;
    bool manualSeedMaxFlag = false;
    bool timestampFlag = false;
    uint32_t seed = 0;
    uint32_t generationDepth = 20;
    uint32_t min_bound = 0;
    uint32_t max_bound = -1;
    bool boundedMinFlag = false;
    bool boundedMaxFlag = false;
    bool generateFlag = false;
    bool bruteforce = false;
    Untwister *untwister = new Untwister();

    while ((c = getopt(argc, argv, "s:S:m:M:D:d:i:g:t:r:c:ubh")) != -1)
    {
        switch (c)
        {
            case 's':
            {
                lowerBoundSeed = strtoll(optarg, NULL, 10);
                manualSeedMinFlag = true;
                break;
            }
            case 'S':
            {
                upperBoundSeed = strtoll(optarg, NULL, 10);
                manualSeedMaxFlag = true;
                break;
            }
            case 'm':
            {
                min_bound = strtoul(optarg, NULL, 10);
                boundedMinFlag = true;
                break;
            }
            case 'M':
            {
                max_bound = strtoul(optarg, NULL, 10);
                boundedMaxFlag = true;
                break;
            }
            case 'g':
            {
                if(optarg != NULL)
                {
                    seed = strtoul(optarg, NULL, 10);
                }
                generateFlag = true;
                break;
            }
            case 'D':
            {
                generationDepth = strtoul(optarg, NULL, 10);
                break;
            }
            case 'u':
            {
                lowerBoundSeed = time(NULL) - ONE_YEAR;
                upperBoundSeed = time(NULL) + ONE_YEAR;
                timestampFlag = true;
                break;
            }
            case 'b':
            {
                bruteforce = true;
                break;
            }
            case 'r':
            {
                if (!untwister->isSupportedPRNG(optarg))
                {
                    std::cerr << WARN << "ERROR: The PRNG \"" << optarg << "\" is not supported, see -h" << std::endl;
                    return EXIT_FAILURE;
                }
                else
                {
                    untwister->setPRNG(optarg);
                }
                break;
            }
            case 'd':
            {
                unsigned int depth = strtoul(optarg, NULL, 10);
                if (depth == 0)
                {
                    std::cerr << WARN << "ERROR: Please enter a valid depth > 1" << std::endl;
                    return EXIT_FAILURE;
                }
                else
                {
                    std::cout << INFO << "Depth set to: " << depth << std::endl;
                    untwister->setDepth(depth);
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
                    /* Ignore lines that start with '#' */
                    if(line.at(0) =='#')
                    {
                        continue;
                    }
                    uint32_t value = strtoul(line.c_str(), NULL, 0);
                    untwister->addObservedOutput(value);
                }
                break;
            }
            case 't':
            {
                unsigned int threads = strtoul(optarg, NULL, 10);
                if (threads == 0)
                {
                    std::cerr << WARN << "ERROR: Please enter a valid number of threads > 1" << std::endl;
                    return EXIT_FAILURE;
                }
                else
                {
                    untwister->setThreads(threads);
                }
                break;
            }
            case 'c':
            {
                double minimumConfidence = ::atof(optarg);
                if (minimumConfidence <= 0 || 100.0 < minimumConfidence)
                {
                    std::cerr << WARN << "ERROR: Invalid confidence percentage " << std::endl;
                    return EXIT_FAILURE;
                }
                else
                {
                    std::cout << INFO << "Minimum confidence set to: " << minimumConfidence << std::endl;
                    untwister->setMinConfidence(minimumConfidence);
                }
                break;
            }
            case 'h':
            {
                Usage(untwister);
                delete untwister;
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
                Usage(untwister);
                delete untwister;
                return EXIT_FAILURE;
            }
            default:
            {
                Usage(untwister);
                delete untwister;
                return EXIT_FAILURE;
            }
        }
    }
    /* You can't just set one bound flag */
    if (boundedMaxFlag ^ boundedMinFlag)
    {
        Usage(untwister);
        std::cerr << WARN << "ERROR: If you want to have a bounded range, provide both -m min and -M max" << std::endl;
        delete untwister;
        return EXIT_SUCCESS;
    }
    /* You can't set both -t (for timestamp) and -s or -S */
    if((manualSeedMaxFlag || manualSeedMinFlag) && timestampFlag)
    {
        Usage(untwister);
        std::cerr << WARN <<
            "ERROR: You can't set both -t (for timestamp based seeding) and manual seeding  with -s or -S" << std::endl;
        delete untwister;
        return EXIT_SUCCESS;
    }
    /* If it wasn't set manually, set the min seed */
    if(!manualSeedMinFlag)
    {
        lowerBoundSeed = untwister->getMinSeed();
    }
    /* If it wasn't set manually, set the max seed */
    if(!manualSeedMaxFlag)
    {
        upperBoundSeed = untwister->getMaxSeed();
    }
    /* Set the bounds, if there any any */
    if (boundedMaxFlag && boundedMinFlag)
    {
        if(max_bound <= min_bound)
        {
            Usage(untwister);
            std::cerr << WARN << "ERROR: Min bound (-m) must be less than max bound (-M)" << std::endl;
            delete untwister;
            return EXIT_SUCCESS;
        }
        try
        {
            untwister->setBounds(min_bound, max_bound);
        }
        catch(const std::string& ex)
        {
            std::cerr << WARN << "ERROR: " << ex << std::endl;
            delete untwister;
            return EXIT_SUCCESS;
        }
    }
    /* If we're generating numbers instead of cracking a seed, then let's do that. */
    if (generateFlag)
    {
        std::vector<uint32_t> results;
        untwister->generateSampleFromSeed(generationDepth, seed);
        delete untwister;
        return EXIT_SUCCESS;
    }
    /* Check input file for contents */
    if (untwister->getObservedOutputs()->empty())
    {
        Usage(untwister);
        std::cerr << WARN << "ERROR: No input numbers provided. Use -i <file> to provide a file" << std::endl;
        delete untwister;
        return EXIT_SUCCESS;
    }
    /* Perform inference attack. Skip it if we're bounded or it was manually skipped */
    if ((boundedMaxFlag && boundedMinFlag) || bruteforce)
    {
        std::cout << INFO << "Skipping inference attack..." << std::endl;
    }
    else
    {
        inferenceAttack(untwister);
    }

    /* Give warning about large seed space */
    if((!boundedMaxFlag || !boundedMinFlag) && (untwister->getPRNG() == "java"))
    {
        std::cout << WARN << "WARNING: Java Random() seeds are 64 bit signed integers. (Though technically they only" <<
        " use 48 bits of it.) Anyway, trying to brute force through all possible seeds will probably never finish. " <<
        "Consider using -s or -S to restrict the seed space to something more reasonable. For instance, Java by" <<
        " default seeds the PRNG with the current system timestamp in milliseconds." <<
        " IE: System.currentTimeMillis()" << std::endl;
    }

    bruteforceAttack(untwister, lowerBoundSeed, upperBoundSeed);
    delete untwister;
    return EXIT_SUCCESS;
}
