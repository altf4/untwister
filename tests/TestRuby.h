#ifndef TESTRUBY_H_
#define TESTRUBY_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../prngs/Ruby.h"

class TestRuby: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TestRuby);
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

#endif /* TESTRUBY_H_ */
