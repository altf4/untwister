/*
 * TestMt19937.cpp
 *
 *  Created on: Dec 13, 2014
 */

#include "TestMt19937.h"

void TestMt19937::setUp()
{

}

void TestMt19937::tearDown()
{

}

void TestMt19937::initalizationTest()
{
    Mt19937 *mt = new Mt19937();
    CPPUNIT_ASSERT(mt != NULL);
    delete mt;
}

void TestMt19937::seedTest()
{
    Mt19937 *mt = new Mt19937();
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->getSeed() == 31337);
    delete mt;
}

void TestMt19937::randomTest()
{
    Mt19937 *mt = new Mt19937();
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->random() == 3100331191);
    CPPUNIT_ASSERT(mt->random() == 3480951327);
    CPPUNIT_ASSERT(mt->random() == 4150831638);
    mt->seed(31337);
    CPPUNIT_ASSERT(mt->random() == 3100331191);
    CPPUNIT_ASSERT(mt->random() == 3480951327);
    CPPUNIT_ASSERT(mt->random() == 4150831638);
    delete mt;
}