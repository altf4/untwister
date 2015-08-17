#ifndef TESTPRNGFACTORY_H_
#define TESTPRNGFACTORY_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../prngs/PRNG.h"
#include "../prngs/PRNGFactory.h"

class TestPRNGFactory: public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TestPRNGFactory);
    CPPUNIT_TEST(initalizationTest);
    CPPUNIT_TEST(getNamesTest);
    CPPUNIT_TEST(getInstanceTest);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void initalizationTest();
    void getNamesTest();
    void getInstanceTest();

private:

};

#endif /* TESTPRNGFACTORY_H_ */
