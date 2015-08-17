#ifndef TESTMT19937_H_
#define TESTMT19937_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../prngs/Mt19937.h"

class TestMt19937: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TestMt19937);
    CPPUNIT_TEST(initalizationTest);
    CPPUNIT_TEST(seedTest);
    CPPUNIT_TEST(randomTest);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void initalizationTest();
    void seedTest();
    void randomTest();

private:

};

#endif /* TESTMT19937_H_ */
