/*
 *
 *  Created on: April 26, 2014
 *      Author: moloch
 *
 *       Notes: Requires Boost C++ libraries
 */

#include <python2.7/Python.h>
#include <boost/python.hpp>

#include "untwister.h"

static const unsigned int TRACE_SIZE = 10;

/* Segfault handler */
void handler(int sig) {
    void *trace[TRACE_SIZE];
    size_t size;
    size = backtrace(trace, TRACE_SIZE);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(trace, size, 2);
    exit(1);
}

/* Python __init__ function (required) */
void python_init()
{
    signal(SIGSEGV, handler);
    if(!Py_IsInitialized())
    {
        Py_Initialize();
        PyEval_InitThreads();
    }
}

boost::python::dict FindSeed(const std::string& rng, unsigned int threads, double miniumConfidence, uint32_t lowerBoundSeed,
        uint32_t upperBoundSeed, uint32_t depth)
{

    SpawnThreads(threads, answers, miniumConfidence, lowerBoundSeed, upperBoundSeed, depth, rng);

    /* Covert to python dictionary */
    return foobar;
}


boost::python::dict mt19932(boost::python::list inputs, unsigned int depth, unsigned int threads)
{
    return FindSeed("mt19932", threads, minimumConfidence, lowerBoundSeed, upperBoundSeed, depth);
}

/* Python interface */
BOOST_PYTHON_MODULE(Untwister) {

    using namespace boost::python;

    def(
        "mt19932",
        mt19932,
        (arg("inputs"), arg("depth") = 10, arg("threads") = 2),
        "Generic mersenne twister 19932"
    );

    def(
        "glibc",
        glibc,
        (arg("inputs"), arg("depth") = 10, arg("threads") = 2),
        "Glibc rand()"
    );

    def(
        "ruby-rand",
        rubyrand,
        (arg("inputs"), arg("depth") = 10, arg("threads") = 2),
        "Ruby's varient of the mersenne twister"
    );

    def("Untwister", python_init);
}