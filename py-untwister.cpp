/*
    Copyright Bishop Fox, 2014

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

    Notes: Requires Boost C++ libraries
*/

#include <python2.7/Python.h>
#include <boost/python.hpp>
#include <execinfo.h>
#include <signal.h>
#include <iostream>

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
    signal(SIGABRT, handler);
    if(!Py_IsInitialized())
    {
        Py_Initialize();
        PyEval_InitThreads();
    }
}

/* Find a seed */
list BruteforceAttack(std::string prng, list observations, unsigned int threads, float minimumConfidence,
    uint32_t lowerBoundSeed, uint32_t upperBoundSeed, uint32_t depth)
{
    Untwister *untwister = new Untwister(len(observations));
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
    for(int index = 0; index < len(observations); ++index)
    {
        uint32_t data = extract<uint32_t>(observations[index]);
        untwister->getObservedOutputs()->at(index) = data;
    }

    /* Suspend Python's thread, so we can use native C++ threads */
    PyThreadState *pyThreadState = PyEval_SaveThread();
    auto results = untwister->bruteforce(lowerBoundSeed, upperBoundSeed);
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


tuple InferStateAttack(std::string prng, list observations, float minimumConfidence)
{
    Untwister *untwister = new Untwister(len(observations));
    if(!untwister->isSupportedPRNG(prng))
    {
        /* Raise Python Exception */
        PyErr_SetString(PyExc_ValueError, "Unsupported PRNG");
        throw error_already_set();
    }

    untwister->setPRNG(prng);

    if(untwister->getStateSize() < (uint32_t) len(observations))
    {
        PyErr_SetString(PyExc_ValueError, "Cannot infer state, too few observations");
        throw error_already_set();
    }

    untwister->setMinConfidence(minimumConfidence);

    /* Convert Python list object to observedOutputs's std::vector<uint32_t> */
    for(int index = 0; index < len(observations); ++index)
    {
        uint32_t data = extract<uint32_t>(observations[index]);
        untwister->getObservedOutputs()->at(index) = data;
    }

    auto state = untwister->inferState();
    list pyState;
    for(unsigned int index = 0; index < state.first.size(); ++index)
    {
        pyState.append(state.first[index]);
    }
    delete untwister;

    return make_tuple(pyState, state.second);
}

/* List all supported PRNGs */
list Prngs()
{
    Untwister *untwister = new Untwister();
    std::vector<std::string> names = untwister->getSupportedPRNGs();

    list pyPRNGs;
    for(unsigned int index = 0; index < names.size(); ++index)
    {
        pyPRNGs.append(names[index]);
    }

    delete untwister;

    return pyPRNGs;
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

    def("get_supported_prngs", Prngs, "\n Get a list of supported PRNGs");

    def(
        "bruteforce",
        BruteforceAttack,
        (arg("prng"), arg("observations"), arg("threads") = threads, arg("confidence") = 100.0, \
            arg("lower") = 0, arg("upper") = UINT_MAX, arg("depth") = 1000),
        "\nThis function attempts to recover a seed using bruteforce for any supported PRNG"
    );

    def(
        "infer_state",
        InferStateAttack,
        (arg("prng"), arg("observations"), arg("confidence") = 100.0),
        "\nThis function attempts to infer the internal state of the PRNG"
    );

}
