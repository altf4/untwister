/*
 * TestPRNGFactory.cpp
 *
 *  Created on: Dec 13, 2014
 */

#include "TestPRNGFactory.h"

void TestPRNGFactory::setUp()
{

}

void TestPRNGFactory::tearDown()
{

}

void TestPRNGFactory::initalizationTest()
{
    PRNGFactory *factory = new PRNGFactory();
    CPPUNIT_ASSERT(factory != NULL);
    delete factory;
}

void TestPRNGFactory::getNamesTest()
{
    PRNGFactory *factory = new PRNGFactory();
    auto names = factory->getNames();
    CPPUNIT_ASSERT(0 < names.size());
    delete factory;
}

void TestPRNGFactory::getInstanceTest()
{
    PRNGFactory *factory = new PRNGFactory();

    PRNG *mt = factory->getInstance("mt19937");
    CPPUNIT_ASSERT(mt->getName() == "mt19937");
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->random() == 3100331191);
    CPPUNIT_ASSERT(mt->random() == 3480951327);
    CPPUNIT_ASSERT(mt->random() == 4150831638);
    delete mt;

    PRNG *php_mt = factory->getInstance("php-mt_rand");
    CPPUNIT_ASSERT(php_mt->getName() == "php-mt_rand");
    php_mt->seed(31337);
    CPPUNIT_ASSERT(php_mt->random() == 590238509);
    CPPUNIT_ASSERT(php_mt->random() == 418805881);
    CPPUNIT_ASSERT(php_mt->random() == 83861629);
    delete php_mt;

    PRNG *ruby = factory->getInstance("ruby-rand");
    CPPUNIT_ASSERT(ruby->getName() == "ruby-rand");
    ruby->seed(31337);
    CPPUNIT_ASSERT(ruby->random() == 3100331191);
    CPPUNIT_ASSERT(ruby->random() == 3480951327);
    CPPUNIT_ASSERT(ruby->random() == 4150831638);
    delete ruby;

    PRNG *glibc = factory->getInstance("glibc-rand");
    CPPUNIT_ASSERT(glibc->getName() == "glibc-rand");
    glibc->seed(31337);
    CPPUNIT_ASSERT(glibc->random() == 53418360);
    CPPUNIT_ASSERT(glibc->random() == 66988840);
    CPPUNIT_ASSERT(glibc->random() == 1189565692);
    delete glibc;

    delete factory;
}
