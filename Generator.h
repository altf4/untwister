#include <stdint.h>
#include <string>
#include <random>

class Generator
{
    public:
        Generator(std::string rng);

        void Seed(uint32_t seed);
        uint32_t Random();

    private:
        uint32_t mt19937_state[625];
        uint32_t *mt19937_next; //sometimes used to mark the next int
        int mt19937_left;

        //mt19937-c++
        std::mt19937 mt19937_generator;

        void mt19937_srand(uint32_t seed);
        uint32_t mt19937_rand();

        //mt19937-php

        void mt19937_php_srand(uint32_t seed);
        uint32_t mt19937_php_rand();

        void php_mt_initialize(uint32_t seed, uint32_t *state);
        void php_mt_reload();

        //glibc-rand
        void glibc_rand_srand(uint32_t seed);
        uint32_t glibc_rand_rand();
};


