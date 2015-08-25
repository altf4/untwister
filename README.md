Untwister
=========

Multi-threaded seed recovery tool for common PRNGs.


Supported PRNGs
=================
* Glibc's rand()
* Mersenne Twister (MT19937)
* PHP's MT-variant (php_mt_rand)
* Ruby's MT-variant DEFAULT::rand()
* Java's Random() class

Usage
========
```
Untwister - Recover PRNG seeds from observed values.
    -i <input_file> [-d <depth> ] [-r <rng_alg>] [-g <seed>] [-t <threads>]

    -i <input_file>
        Path to file input file containing observed results of your RNG. The contents
        are expected to be newline separated 32-bit integers. See test_input.txt for
        an example.
    -d <depth>
        The depth (default 1000) to inspect for each seed value when brute forcing.
        Choosing a higher depth value will make brute forcing take longer (linearly),
        but is required for cases where the generator has been used many times already.
        Also controls how deep to generate random numbers given the -g option
    -r <rng_alg>
        The RNG algorithm to use. Supported RNG algorithms:
        glibc-rand (default)
        mt19937
        php-mt_rand
        ruby-rand
        java
    -u
        Use bruteforce, but only for unix timestamp values within a range of +/- 1
        year from the current time.
    -b
        Always bruteforce, even if state inference attack is successful
    -g <seed>
        Generate a test set of random numbers from the given seed
    -D
        The quantity of random numbers to generate when using the -g flag (default 20)
    -c <confidence>
        Set the minimum confidence percentage to report
    -t <threads>
        Spawn this many threads (default is your number of CPUs)
    -m <min bound>
        Set the minimum bound (inclusive), for a bounded PRNG function
    -M <max bound>
        Set the maximum bound (inclusive), for a bounded PRNG function
    -s <min seed>
        Set the minimum seed (inclusive), for brute forcing (a 64 bit signed integer)
    -S <max seed>
        Set the maxmimum seed (inclusive), for brute forcing (a 64 bit signed integer)

Examples:
    Cracking a list of random numbers in test_ints.txt
        ./untwister -i test_ints.txt

    Generating 70 random numbers from seed 1234, using glibc-rand
        ./untwister -d 70 -g 1234 -r glibc-rand

    Generating 90 random numbers from state file saved in state.txt
        ./untwister -d 90 -g -i state.txt
```


Python Bindings
=================
* Python 2.7
* Requires Boost C++ Python library

### Ubuntu/Debian Linux

```
sudo apt-get install python-dev libboost-python-dev libcppunit-dev
make python
```

### OSX

```
brew install boost --with-python
make python
```


Example script:

```
#!/usr/bin/env python
import untwister

with open('observed_ints.txt') as fp:
    sample = [int(line) for line in fp.readlines()]
    results = untwister.bruteforce(untwister.MT19937, sample, threads=4)
    print results  # We get back a list of tuples
```
