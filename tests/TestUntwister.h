#ifndef TESTUNTWISTER_H_
#define TESTUNTWISTER_H_

#include <thread>
#include <exception>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../Untwister.h"

static const unsigned int TEST_COUNT = 10;


class TestUntwister: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TestUntwister);
    CPPUNIT_TEST(initalizationTest);
    CPPUNIT_TEST(setThreadsTest);
    CPPUNIT_TEST(setDepthTest);
    CPPUNIT_TEST(setMinConfidenceTest);
    CPPUNIT_TEST(setPRNGTest);
    CPPUNIT_TEST(mtBruteforceTest);
    CPPUNIT_TEST(glibcBruteforceTest);
    CPPUNIT_TEST(rubyBruteforceTest);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void initalizationTest();
    void setThreadsTest();
    void setDepthTest();
    void setMinConfidenceTest();
    void setPRNGTest();
    void mtBruteforceTest();
    void glibcBruteforceTest();
    void rubyBruteforceTest();

private:
    std::vector<uint32_t> *m_mtTestInputs;
    std::vector<uint32_t> *m_glibcTestInputs;
    std::vector<uint32_t> *m_rubyTestInputs;
    void m_loadTestFile(std::string path, std::vector<uint32_t> *testInputs);
};

#endif /* TESTUNTWISTER_H_ */
