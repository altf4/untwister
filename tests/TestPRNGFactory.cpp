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
    delete factory;
}

