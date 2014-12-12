/*
 *
 *  Created on: April 26, 2014
 *      Author: moloch
 *
 *       Notes: Requires Boost C++ libraries
 */

#include <python2.7/Python.h>
#include <boost/python.hpp>
#include <execinfo.h>
#include <signal.h>

#include "Untwister.h"

using namespace boost::python;

static const unsigned int TRACE_SIZE = 10;

/* Segfault handler - for debugging only */
void handler(int sig)
{
    void *trace[TRACE_SIZE];
    size_t size = backtrace(trace, TRACE_SIZE);
    std::cerr << "[!] SIGSEGV: " << sig << std::endl;
    backtrace_symbols_fd(trace, size, 2);
    exit(1);
}

/* Python __init__ function */
void PythonInit()
{
    signal(SIGSEGV, handler);
    if(!Py_IsInitialized())
    {
        Py_Initialize();
        PyEval_InitThreads();
    }
}

/* Find a seed */
list FindSeed(const std::string& prng, list inputs, unsigned int threads, float minimumConfidence,
    uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth)
{
    Untwister *untwister = new Untwister(len(inputs));
    if(!untwister->isSupportedPRNG(prng))
    {
        /* Raise Python Exception */
        PyErr_SetString(PyExc_ValueError, "Unsupported PRNG");
        throw error_already_set();
    }
    untwister->setPRNG(prng);
    untwister->setThreads(threads);
    untwister->setDepth(depth);
    untwister->setMinConfidence(minimumConfidence);

    /* Convert Python list object to observedOutputs's std::vector<uint32_t> */
    for(unsigned int index = 0; index < len(inputs); ++index)
    {
        uint32_t data = boost::python::extract<unsigned int>(inputs[index]);
        untwister->getObservedOutputs()->at(index) = data;
    }

    /* Suspend Python's thread, so we can use native C++ threads */
    PyThreadState *pyThreadState = PyEval_SaveThread();

    std::vector<std::pair<uint32_t, double> > results = untwister->bruteforce(lowerBoundSeed, upperBoundSeed);


    /* Clean up and restore Python thread state */
    PyEval_RestoreThread(pyThreadState);

    /* Covert answers to python list of tuples */
    list pyResults;
    for(unsigned int index = 0; index < results.size(); ++index)
    {
        tuple pySeed = make_tuple(results[index].first, results[index].second);
        pyResults.append(pySeed);
    }
    delete untwister;
    return pyResults;
}

/* Generate a sample sequence */
list Sample(std::string prng, uint32_t seed, uint32_t depth)
{
    Untwister *untwister = new Untwister();
    if(!untwister->isSupportedPRNG(prng))
    {
        /* Raise Python Exception */
        PyErr_SetString(PyExc_ValueError, "Unsupported PRNG");
        throw error_already_set();
    }

    untwister->setPRNG(prng);
    untwister->setDepth(depth);

    std::vector<uint32_t> results = untwister->generateSampleFromSeed(seed);
    list sample;
    for(unsigned int index = 0; index < results.size(); ++index)
    {
        sample.append(results.at(index));
    }
    return sample;
}

/* List all supported PRNGs */
list Prngs()
{
    Untwister *untwister = new Untwister();
    std::vector<std::string> names = untwister->getPRNGNames();
    list prngs;
    for(unsigned int index = 0; index < names.size(); ++index)
    {
        prngs.append(names[index]);
    }
    delete untwister;
    return prngs;
}

/* Python interface */
BOOST_PYTHON_MODULE(untwister) {

    def("untwister", PythonInit);

    scope current;
    current.attr("__doc__") = "Multi-threaded seed recovery tool for common PRNGs";
    current.attr("MT19937") = "mt19937";
    current.attr("GLIBC") = "glibc-rand";
    current.attr("RUBY") = "ruby-rand";
    unsigned int threads = std::thread::hardware_concurrency();
    current.attr("THREADS") = threads;

    def("get_prngs", Prngs, "\n List of supported PRNGs");

    def(
        "find_seed",
        FindSeed,
        (arg("prng"), arg("inputs"), arg("threads") = threads, arg("confidence") = 100.0, \
            arg("lower") = 0, arg("upper") = UINT_MAX, arg("depth") = 1000),
        "\nThis is the cracking module for a generic mersenne twister 19932"
    );

    def(
        "generate_sample",
        Sample,
        (arg("prng"), arg("seed"), arg("depth") = 1000),
        "\n Generate a sample using a given PRNG"
    );

}