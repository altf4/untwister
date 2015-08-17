#ifndef TEST_PHP_MT_RAND_
#define TEST_PHP_MT_RAND_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../prngs/PHP_mt19937.h"

class Test_PHP_Mt19937: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Test_PHP_Mt19937);
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

#endif /* TEST_PHP_MT_RAND_ */
