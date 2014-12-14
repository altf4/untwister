/*
 * TestUntwister.h
 *
 *  Created on: Dec 13, 2014
 */

#ifndef TESTUNTWISTER_H_
#define TESTUNTWISTER_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../Untwister.h"

class TestUntwister: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TestUntwister);
    CPPUNIT_TEST(initalizationTest);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void initalizationTest();

private:
    std::vector<uint32_t> *m_mtTestInputs;
    std::vector<uint32_t> *m_glibcTestInputs;
    std::vector<uint32_t> *m_rubyTestInputs;

    void loadTestFile(std::string path, std::vector<uint32_t> *testInputs);

};

#endif /* TESTUNTWISTER_H_ */