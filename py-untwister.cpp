/*
 *
 *  Created on: April 26, 2014
 *      Author: moloch
 *
 *       Notes: Requires Boost C++ libraries
 */

#include <python2.7/Python.h>
#include <boost/python.hpp>
#include <thread>

#include "untwister.h"

using namespace boost::python;

static const unsigned int TRACE_SIZE = 10;


/* Python __init__ function */
void python_init()
{
    if (!Py_IsInitialized())
    {
        Py_Initialize();
        PyEval_InitThreads();
    }
}

/*  Python Threading - eventually we'll want to refactor this code into the main
 *  untwister.h but for now we just want it working.
 *
 */
void SpawnThreads(const unsigned int threads, std::vector<std::vector<Seed>* > *answers, double minimumConfidence,
        uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth, std::string rng)
{
    /* Suspend Python's thread, so we can use native C++ threads */
    PyThreadState* pyThreadState = PyEval_SaveThread();

    bool isCompleted = false;
    std::vector<std::thread> pool(threads);
    std::vector<uint32_t> *status = new std::vector<uint32_t>(threads);
    std::vector<uint32_t> labor = DivisionOfLabor(upperBoundSeed - lowerBoundSeed, threads);
    uint32_t startAt = lowerBoundSeed;

    for (unsigned int id = 0; id < threads; ++id)
    {
        uint32_t endAt = startAt + labor.at(id);
        pool[id] = std::thread(BruteForce, id, std::ref(isCompleted), answers, status, minimumConfidence, startAt, endAt, depth, rng);
        startAt += labor.at(id);
    }

    for (unsigned int id = 0; id < pool.size(); ++id)
    {
        pool[id].join();
    }

    /* Clean up and restore Python thread state */
    PyEval_RestoreThread(pyThreadState);
    pyThreadState = NULL;
    delete status;
}

list FindSeed(const std::string& rng, list inputs, unsigned int threads, float minimumConfidence,
    uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth)
{
    /* Convert Python list object to observedOutputs's std::vector<uint32_t> */
    for (unsigned int index = 0; index < len(inputs); ++index) {
        unsigned int data = boost::python::extract<unsigned int>(inputs[index]);
        observedOutputs.push_back(data);
    }

    /* Each thread needs their own set of answers to avoid locking */
    std::vector<std::vector<Seed>* > *answers = new std::vector<std::vector<Seed>* >(threads);
    SpawnThreads(threads, answers, (double) minimumConfidence, lowerBoundSeed, upperBoundSeed, depth, rng);

    /* Covert answers to python list of tuples */
    list results;
    for (unsigned int id = 0; id < answers->size(); ++id)
    {
        /* Look for answers from each thread */
        for (unsigned int index = 0; index < answers->at(id)->size(); ++index)
        {
            tuple result = make_tuple(answers->at(id)->at(index).first, answers->at(id)->at(index).second);
            results.append(result);
        }
        delete answers->at(id);
    }
    delete answers;
    return results;
}


list crack_mt19932(list inputs, unsigned int threads, float minimumConfidence, uint32_t lowerBoundSeed,
        uint32_t upperBoundSeed, uint32_t depth)
{
    return FindSeed("mt19932", inputs, threads, minimumConfidence, lowerBoundSeed, upperBoundSeed, depth);
}

list crack_glibc(list inputs, unsigned int threads, float minimumConfidence, uint32_t lowerBoundSeed,
        uint32_t upperBoundSeed, uint32_t depth)
{
    return FindSeed("glibc", inputs, threads, minimumConfidence, lowerBoundSeed, upperBoundSeed, depth);
}

/* Python interface */
BOOST_PYTHON_MODULE(untwister) {

    def(
        "mt19932",
        crack_mt19932,
        (arg("inputs"), arg("threads") = 2, arg("minimumConfidence") = 100.0, arg("lowerBoundSeed") = 0, arg("upperBoundSeed") = UINT_MAX, arg("depth") = 1000),
        "\nThis is the cracking module for a generic mersenne twister 19932"
    );

    def(
        "glibc",
        crack_glibc,
        (arg("inputs"), arg("threads") = 2, arg("minimumConfidence") = 100.0, arg("lowerBoundSeed") = 0, arg("upperBoundSeed") = UINT_MAX, arg("depth") = 1000),
        "\nThis is the cracking module for a the generic glibc rand()"
    );

    def("untwister", python_init);
}