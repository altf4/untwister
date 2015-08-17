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
*/

#include "TestUntwister.h"
#include <stdexcept>

void TestUntwister::setUp()
{
    m_mtTestInputs = new std::vector<uint32_t>();
    m_loadTestFile("tests/mt_test_ints.txt", m_mtTestInputs);

    m_glibcTestInputs = new std::vector<uint32_t>();
    m_loadTestFile("tests/glibc_test_ints.txt", m_glibcTestInputs);

    m_rubyTestInputs = new std::vector<uint32_t>();
    m_loadTestFile("tests/ruby_test_ints.txt", m_rubyTestInputs);
}

void TestUntwister::tearDown()
{
    delete m_mtTestInputs;
    delete m_glibcTestInputs;
    delete m_rubyTestInputs;
}

void TestUntwister::m_loadTestFile(std::string path, std::vector<uint32_t> *testInputs)
{
    std::ifstream infile(path);
    if (!infile)
    {
        std::cerr << WARN << "ERROR: File \"" << path << "\" not found" << std::endl;
    }
    std::string line;
    while (std::getline(infile, line))
    {
        uint32_t value = strtoul(line.c_str(), NULL, 0);
        testInputs->push_back(value);
    }
}

void TestUntwister::initalizationTest()
{
    Untwister *untwister = new Untwister();
    CPPUNIT_ASSERT(untwister != NULL);
    delete untwister;
}

void TestUntwister::setThreadsTest()
{
    Untwister *untwister = new Untwister();
    untwister->setThreads(1);
    CPPUNIT_ASSERT(untwister->getThreads() == 1);
    untwister->setThreads(2);
    CPPUNIT_ASSERT(untwister->getThreads() == 2);
    untwister->setThreads(3);
    CPPUNIT_ASSERT(untwister->getThreads() == 3);
    untwister->setThreads(4);
    CPPUNIT_ASSERT(untwister->getThreads() == 4);
    delete untwister;
}

void TestUntwister::setDepthTest()
{
    Untwister *untwister = new Untwister();
    untwister->setDepth(100);
    CPPUNIT_ASSERT(untwister->getDepth() == 100);
    untwister->setDepth(200);
    CPPUNIT_ASSERT(untwister->getDepth() == 200);
    untwister->setDepth(3000);
    CPPUNIT_ASSERT(untwister->getDepth() == 3000);
    untwister->setDepth(4000);
    CPPUNIT_ASSERT(untwister->getDepth() == 4000);
    delete untwister;
}

void TestUntwister::setMinConfidenceTest()
{
    Untwister *untwister = new Untwister();
    untwister->setMinConfidence(99.0);
    CPPUNIT_ASSERT(untwister->getMinConfidence() == 99.0);
    untwister->setMinConfidence(50.0);
    CPPUNIT_ASSERT(untwister->getMinConfidence() == 50.0);
    untwister->setMinConfidence(75.75);
    CPPUNIT_ASSERT(untwister->getMinConfidence() == 75.75);
    untwister->setMinConfidence(100.0);
    CPPUNIT_ASSERT(untwister->getMinConfidence() == 100.0);
    delete untwister;
}

void TestUntwister::setPRNGTest()
{
    Untwister *untwister = new Untwister();
    untwister->setPRNG(std::string("mt19937"));
    CPPUNIT_ASSERT(untwister->getPRNG() == "mt19937");
    CPPUNIT_ASSERT_THROW(untwister->setPRNG(std::string("foobar")), std::runtime_error);
    delete untwister;
}

void TestUntwister::mtBruteforceTest()
{
    Untwister *untwister = new Untwister();
    for (unsigned int index = 0; index < m_mtTestInputs->size(); ++index)
    {
        untwister->addObservedOutput(m_mtTestInputs->at(index));
    }
    CPPUNIT_ASSERT(0 < untwister->getObservedOutputs()->size());
    untwister->setPRNG(std::string("mt19937"));

    for (unsigned int index = 0; index < TEST_COUNT; ++index)
    {
        uint32_t start = 100 * TEST_COUNT;
        auto results = untwister->bruteforce(start, 50000);

        CPPUNIT_ASSERT(0 < results.size());
        if (0 < results.size())
        {
            CPPUNIT_ASSERT(results[0].first == 31337);
            CPPUNIT_ASSERT(results[0].second == 100.0);
        }
    }

    auto results2 = untwister->bruteforce(50000, 100000);
    CPPUNIT_ASSERT(0 == results2.size());

    delete untwister;
}

void TestUntwister::glibcBruteforceTest()
{
    Untwister *untwister = new Untwister();
    for (unsigned int index = 0; index < m_glibcTestInputs->size(); ++index)
    {
        untwister->addObservedOutput(m_glibcTestInputs->at(index));
    }
    CPPUNIT_ASSERT(0 < untwister->getObservedOutputs()->size());
    untwister->setPRNG(std::string("glibc-rand"));

    for (unsigned int index = 0; index < TEST_COUNT; ++index)
    {
        uint32_t start = 100 * TEST_COUNT;
        auto results = untwister->bruteforce(start, 50000);

        CPPUNIT_ASSERT(0 < results.size());
        if (0 < results.size())
        {
            CPPUNIT_ASSERT(results[0].first == 1337);
            CPPUNIT_ASSERT(results[0].second == 100.0);
        }
    }

    auto results2 = untwister->bruteforce(50000, 100000);
    CPPUNIT_ASSERT(0 == results2.size());

    delete untwister;
}

void TestUntwister::rubyBruteforceTest()
{
    Untwister *untwister = new Untwister();
    for (unsigned int index = 0; index < m_rubyTestInputs->size(); ++index)
    {
        untwister->addObservedOutput(m_rubyTestInputs->at(index));
    }
    CPPUNIT_ASSERT(0 < untwister->getObservedOutputs()->size());
    untwister->setPRNG(std::string("ruby-rand"));

    for (unsigned int index = 0; index < TEST_COUNT; ++index)
    {
        uint32_t start = 100 * TEST_COUNT;
        auto results = untwister->bruteforce(start, 50000);

        CPPUNIT_ASSERT(0 < results.size());
        if (0 < results.size())
        {
            CPPUNIT_ASSERT(results[0].first == 31337);
            CPPUNIT_ASSERT(results[0].second == 100.0);
        }
    }

    auto results2 = untwister->bruteforce(50000, 100000);
    CPPUNIT_ASSERT(0 == results2.size());
    delete untwister;
}
