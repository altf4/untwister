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
        //mt19937
        std::mt19937 mt19937_generator;

        void mt19937_srand(uint32_t seed);
        uint32_t mt19937_rand();

        //glibc-rand
        void glibc_rand_srand(uint32_t seed);
        uint32_t glibc_rand_rand();
};


