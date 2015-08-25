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

#include "TestJava.h"

void TestJava::setUp()
{

}

void TestJava::tearDown()
{

}

void TestJava::initalizationTest()
{
    Java *java = new Java();
    CPPUNIT_ASSERT(java != NULL);
    delete java;
}

void TestJava::seedTest()
{
    Java java = Java();
    java.seed(31337);
    CPPUNIT_ASSERT(java.getSeed() == 31337);
}

void TestJava::randomTest()
{
    Java java = Java();
    java.seed(1337);
    CPPUNIT_ASSERT(java.random() == 2834376842);
    CPPUNIT_ASSERT(java.random() == 747279288);
    CPPUNIT_ASSERT(java.random() == 2960274719);
    java.seed(1337);
    CPPUNIT_ASSERT(java.random() == 2834376842);
    CPPUNIT_ASSERT(java.random() == 747279288);
    CPPUNIT_ASSERT(java.random() == 2960274719);

    /* Bounded Java rand(int, int) calls */
    java.setBounds(0, 99);
    java.seed(31337);
    CPPUNIT_ASSERT(java.random() == 9);
    CPPUNIT_ASSERT(java.random() == 73);
    CPPUNIT_ASSERT(java.random() == 49);
    java.setBounds(0, 99);
    java.seed(31337);
    CPPUNIT_ASSERT(java.random() == 9);
    CPPUNIT_ASSERT(java.random() == 73);
    CPPUNIT_ASSERT(java.random() == 49);
}
