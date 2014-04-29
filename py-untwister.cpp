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

#include "untwister.h"

using namespace boost::python;

static const unsigned int TRACE_SIZE = 10;

/* Segfault handler - for debugging only */
void handler(int sig) {
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

/* Raises a Python exception if the string is not a supported prng */
void CheckPRNG(std::string prng)
{
    PRNGFactory factory;
    std::vector<std::string> names = factory.getNames();
    if(std::find(names.begin(), names.end(), prng) == names.end())
    {
        /* Raise Python Exception */
        PyErr_SetString(PyExc_ValueError, "Unsupported PRNG");
        throw error_already_set();
    }
}

/* Find a seed */
list FindSeed(const std::string& rng, list inputs, unsigned int threads, float minimumConfidence,
    uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth)
{
    CheckPRNG(rng);

    /* Convert Python list object to observedOutputs's std::vector<uint32_t> */
    for(unsigned int index = 0; index < len(inputs); ++index) {
        unsigned int data = boost::python::extract<unsigned int>(inputs[index]);
        observedOutputs.push_back(data);
    }

    std::vector<std::vector<Seed>* > *answers = new std::vector<std::vector<Seed>* >(threads);

    /* Suspend Python's thread, so we can use native C++ threads */
    PyThreadState* pyThreadState = PyEval_SaveThread();
    StartBruteForce(threads, answers, (double) minimumConfidence, lowerBoundSeed, upperBoundSeed, depth, rng);

    /* Clean up and restore Python thread state */
    PyEval_RestoreThread(pyThreadState);
    pyThreadState = NULL;

    /* Covert answers to python list of tuples */
    list results;
    for(unsigned int id = 0; id < answers->size(); ++id)
    {
        for(unsigned int index = 0; index < answers->at(id)->size(); ++index)
        {
            tuple result = make_tuple(answers->at(id)->at(index).first, answers->at(id)->at(index).second);
            results.append(result);
        }
        delete answers->at(id);
    }
    delete answers;
    return results;
}

/* Generate a sample sequence */
list Sample(std::string prng, uint32_t seed, uint32_t depth)
{
    CheckPRNG(prng);

    std::vector<uint32_t> results = GenerateSample(seed, depth, prng);
    /* Convert results to a Python object */
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
    PRNGFactory factory;
    std::vector<std::string> names = factory.getNames();
    list prngs;
    for(unsigned int index = 0; index < names.size(); ++index)
    {
        prngs.append(names.at(index));
    }
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

    def("prngs", Prngs, "\n List of supported PRNGs");

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