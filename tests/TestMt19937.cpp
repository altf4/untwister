/*
    Copyright Bishop Fox, 2014

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
