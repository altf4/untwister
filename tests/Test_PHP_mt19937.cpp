#include "Test_PHP_mt19937.h"

void Test_PHP_Mt19937::setUp()
{

}

void Test_PHP_Mt19937::tearDown()
{

}

void Test_PHP_Mt19937::initalizationTest()
{
    PHP_mt19937 *mt = new PHP_mt19937();
    CPPUNIT_ASSERT(mt != NULL);
    delete mt;
}

void Test_PHP_Mt19937::seedTest()
{
    PHP_mt19937 *mt = new PHP_mt19937();
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->getSeed() == 31337);
    delete mt;
}

void Test_PHP_Mt19937::randomTest()
{
    PHP_mt19937 *mt = new PHP_mt19937();
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->random() == 590238509);
    CPPUNIT_ASSERT(mt->random() == 418805881);
    CPPUNIT_ASSERT(mt->random() == 83861629);
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->random() == 590238509);
    CPPUNIT_ASSERT(mt->random() == 418805881);
    CPPUNIT_ASSERT(mt->random() == 83861629);
    delete mt;
}
