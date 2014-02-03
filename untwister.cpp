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
#include <random>
#include <fstream>
#include <vector>
#include <limits.h>

using namespace std;

//seed, quality of fit
typedef pair<uint32_t, double> Seed; 

static uint32_t depth = 1000;
static vector<uint32_t> observed_outputs;

void Usage()
{
    cout << "Untwister, recover your PRNG seeds from observed values." << endl;
    cout << "\t-d <depth> -i <input_file> [-g <seed>]" << endl;
}

vector <Seed> BruteForce()
{
    vector <Seed> answers;

    //TODO: Technically, this will miss the last seed value
    for(uint32_t i = 0; i < ULONG_MAX; i++)
    {
        mt19937 generator(i);
        int matchesFound = 0;
        for(uint32_t j = 0; j < depth; j++)
        {
            uint32_t next_rand = generator();
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
        else
        {
            //cout << "failed: " << i << endl;
        }
    }
    return answers;
}

void GenerateSample(uint32_t seed)
{
    mt19937 generator(seed);
    mt19937 distance_gen(time(NULL));
    uint32_t distance = distance_gen() % (depth - 10);

    //Burn a bunch of random numbers
    for(int i = 0; i < distance; i++)
    {
        generator();
    }

    for(int i = 0; i < 10; i++)
    {
        cout << generator() << endl;
    }
}

int main(int argc, char **argv)
{
    int c;
    while ((c = getopt (argc, argv, "d:i:g:")) != -1)
    {
        switch (c)
        {
            case 'g':
            {
                uint32_t seed = strtoul(optarg, NULL, 10);
                GenerateSample(seed);
                return EXIT_SUCCESS;
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

    if(observed_outputs.empty())
    {
        Usage();
        cerr << "ERROR: No input numbers provided. Use -i <file> to provide a file" << endl;
        return EXIT_FAILURE;
    }

    vector <Seed> answers = BruteForce();
    for(int i = 0; i < answers.size(); i++)
    {
        cout << "Possible seed: " << answers[i].first;
        cout << " with strength: " << answers[i].second << endl;
    }
    return EXIT_SUCCESS;
}

