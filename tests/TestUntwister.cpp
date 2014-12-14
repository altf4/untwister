/*
 * TestUntwister.cpp
 *
 *  Created on: Dec 13, 2014
 */

#include "TestUntwister.h"

void TestUntwister::setUp()
{
    m_mtTestInputs = new std::vector<uint32_t>();
    m_glibcTestInputs = new std::vector<uint32_t>();
    m_rubyTestInputs = new std::vector<uint32_t>();
}

void TestUntwister::tearDown()
{
    delete m_mtTestInputs;
    delete m_glibcTestInputs;
    delete m_rubyTestInputs;
}

void TestUntwister::loadTestFile(std::string path, std::vector<uint32_t> *testInputs)
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

}