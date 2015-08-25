#ifndef TESTJAVA_H_
#define TESTJAVA_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../prngs/Java.h"

class TestJava: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TestJava);
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

#endif /* TESTJAVA_H_ */
