Untwister
=========

Multi-threaded seed recovery tool for common PRNGs

Supported PRNGs
=================
* Glibc rand()
* Mersenne Twister (mt19937)
* Ruby's MT-variant rand()

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
        Choosing a higher depth value will make brute forcing take longer (linearly), but is required for cases where the generator has been used many times already.
    -r <rng_alg>
        The RNG algorithm to use. Supported RNG algorithms:
        glibc-rand (default)
        mt19937
        ruby-rand
    -u
        Use bruteforce, but only for unix timestamp values within a range of +/- 1
        year from the current time.
    -g <seed>
        Generate a test set of random numbers from the given seed (at a random depth)
    -t <threads>
        Spawn this many threads (default is 4)
```


Python Bindings
=================

* Requires Boost C++ Python library
* Python 2.7

```
brew install boost
make python
```


Example script:

```
#!/usr/bin/env python
import untwister

with open('test_ints.txt') as fp:
    sample = [int(line) for line in fp.readlines()]
    results = untwister.find_seed(untwister.MT19937, sample, threads=4)
    print results  # We get back a list of tuples
```
