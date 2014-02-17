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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include "Generator.h"

using namespace std;

//Pair of <seed, quality of fit>
typedef pair<uint32_t, double> Seed; 
static vector<uint32_t> observed_outputs;

void Usage()
{
    cout << "Untwister, recover PRNG seeds from observed values." << endl;
    cout << "\t-i <input_file> [-d <depth> ] [-r <rng_alg>] [-g <seed>] [-t]\n" << endl;
    cout << "\t-i <input_file>\n\t\tPath to file input file containing observed results of your RNG. The contents" << endl;
    cout << "\t\tare expected to be newline separated 32-bit integers. See test_input.txt for" << endl;
    cout << "\t\tan example." << endl;
    cout << "\t-d <depth>\n\t\tThe depth (default 1000) to inspect for each seed value when brute forcing." << endl;
    cout << "\t\tChoosing a higher depth value will make brute forcing take longer (linearly), but is " << endl;
    cout << "\t\trequired for cases where the generator has been used many times already." << endl;
    cout << "\t-r <rng_alg>\n\t\tThe RNG algorithm to use. Supported RNG algorithms:" << endl;
    cout << "\t\t\tmt19937 (default)" << endl;
    cout << "\t\t\tglibc-rand" << endl;
    cout << "\t-t\n\t\tUse bruteforce, but only for unix timestamp values within a range of +/- 1 " << endl;
    cout << "\t\tyear from the current time." << endl;
    cout << "\t-g <seed>\n\t\tGenerate a test set of random numbers from the given seed (at a random depth)" << endl;
    cout << "" << endl;

}

vector <Seed> BruteForce(uint32_t starting_seed, uint32_t ending_seed, 
    uint32_t depth, string rng)
{
    vector <Seed> answers;
    Generator generator(rng);

    //TODO: Technically, this will miss the last seed value
    for(uint32_t i = starting_seed; i < ending_seed; i++)
    {
        generator.Seed(i);        

        uint32_t matchesFound = 0;
        for(uint32_t j = 0; j < depth; j++)
        {
            uint32_t next_rand = generator.Random();
            uint32_t observed = observed_outputs[matchesFound];

            if(observed == next_rand)
            {
                matchesFound++;
                if(matchesFound == observed_outputs.size())
                {
                    //This seed is a winner if we get to the end. Just quit the loop
                    break;
                }
            }
        }
        if(matchesFound == observed_outputs.size())
        {
            Seed seed = {i, 100};
            answers.push_back(seed);
            cout << "success!: " << i << endl;
        }
    }
    return answers;
}

void GenerateSample(uint32_t seed, uint32_t depth, string rng)
{
    Generator generator(rng);
    generator.Seed(seed);
    Generator distance_gen(rng);
    distance_gen.Seed(time(NULL));
    uint32_t distance = distance_gen.Random() % (depth - 10);

    //Burn a bunch of random numbers
    for(uint32_t i = 0; i < distance; i++)
    {
        generator.Random();
    }

    for(uint32_t i = 0; i < 10; i++)
    {
        cout << generator.Random() << endl;
    }
}

int main(int argc, char **argv)
{
    int c;
    uint32_t starting_seed = 0;
    uint32_t ending_seed = ULONG_MAX;
    uint32_t depth = 1000;
    string rng = "mt19937";
    uint32_t seed = 0;

    while ((c = getopt (argc, argv, "d:i:g:tr:")) != -1)
    {
        switch (c)
        {
            case 'g':
            {
                seed = strtoul(optarg, NULL, 10);
                break;
            }
            case 't':
            {
                //Default behavior of the -t flag is to try all timestamp seeds
                //within a +/- 1 year timeframe from the present.
                starting_seed = time(NULL) - 31536000;
                ending_seed = time(NULL) + 31536000;
                break;
            }
            case 'r':
            {
                rng = optarg;
                break;
            }
            case 'd':
            {
                depth = strtoul(optarg, NULL, 10);
                if(depth == 0)
                {
                    cerr << "ERROR: Please enter a valid depth > 1" << endl;
                    return EXIT_FAILURE;
                }
                break;
            }
            case 'i':
            {
                ifstream infile(optarg);
                if(!infile)
                {
                    cerr << "ERROR: File \"" << optarg << "\" not found" << endl;
                }
                std::string line;
                while (std::getline(infile, line))
                {
                    observed_outputs.push_back(strtoul(line.c_str(), NULL, 10));
                }
                break;
            }
            case '?':
            {
                if (optopt == 'd')
                   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                   fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                Usage();
                return EXIT_FAILURE;
            }            
            default:
            {
                Usage();
                return EXIT_FAILURE;
            }
        }
    }

    if(seed != 0)
    {
        GenerateSample(seed, depth, rng);
        return EXIT_SUCCESS;
    }

    if(observed_outputs.empty())
    {
        Usage();
        cerr << "ERROR: No input numbers provided. Use -i <file> to provide a file" << endl;
        return EXIT_FAILURE;
    }

    vector <Seed> answers = BruteForce(starting_seed, ending_seed, depth, rng);
    for(uint32_t i = 0; i < answers.size(); i++)
    {
        cout << "Possible seed: " << answers[i].first;
        cout << " with strength: " << answers[i].second << endl;
    }
    return EXIT_SUCCESS;
}

