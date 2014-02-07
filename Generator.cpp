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

#include <stdlib.h>
#include "Generator.h"
#include <iostream>

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

using namespace std;

typedef void (Generator::*GeneratorSeedFn)(uint32_t seed);
typedef uint32_t (Generator::*GeneratorRandFn)(void);

GeneratorSeedFn untwister_srand;
GeneratorRandFn untwister_rand;

Generator::Generator(std::string rng)
{
    if(rng.compare("mt19937") == 0)
    {
        untwister_srand = &Generator::mt19937_srand;
        untwister_rand = &Generator::mt19937_rand;
    }
    else if(rng.compare("glibc-rand") == 0)
    {
        untwister_srand = &Generator::glibc_rand_srand;
        untwister_rand = &Generator::glibc_rand_rand;
    }
    else
    {
        //Error case
        cerr << "ERROR: Unknown rng type selected" << endl;
        exit(EXIT_FAILURE);
    }
}

void Generator::Seed(uint32_t seed)
{
    CALL_MEMBER_FN(*this, untwister_srand)(seed);
}

uint32_t Generator::Random()
{
    return CALL_MEMBER_FN(*this, untwister_rand)();
}

void Generator::mt19937_srand(uint32_t seed)
{
    mt19937_generator.seed(seed);
}

uint32_t Generator::mt19937_rand()
{
    return mt19937_generator();
}

void Generator::glibc_rand_srand(uint32_t seed)
{
    srand(seed);
}

uint32_t Generator::glibc_rand_rand()
{
    return rand();
}

