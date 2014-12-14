/*
 * TestRuby.cpp
 *
 *  Created on: Dec 13, 2014
 */

#include "TestRuby.h"

void TestRuby::setUp()
{

}

void TestRuby::tearDown()
{

}

void TestRuby::initalizationTest()
{
    Ruby *ruby = new Ruby();
    CPPUNIT_ASSERT(ruby != NULL);
    delete ruby;
}

void TestRuby::seedTest()
{
    Ruby *ruby = new Ruby();
    ruby->seed(31337);
    CPPUNIT_ASSERT(ruby->getSeed() == 31337);
    delete ruby;
}

void TestRuby::randomTest()
{
    Ruby *ruby = new Ruby();
    ruby->seed(31337);
    CPPUNIT_ASSERT(ruby->random() == 3100331191);
    CPPUNIT_ASSERT(ruby->random() == 3480951327);
    CPPUNIT_ASSERT(ruby->random() == 4150831638);
    ruby->seed(31337);
    CPPUNIT_ASSERT(ruby->random() == 3100331191);
    CPPUNIT_ASSERT(ruby->random() == 3480951327);
    CPPUNIT_ASSERT(ruby->random() == 4150831638);
    delete ruby;
}