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

    /* Bounded Ruby rand(int, int) calls */
    ruby->setBounds(1234, 9876);
    ruby->seed(6789);
    CPPUNIT_ASSERT(ruby->random() == 4081);
    CPPUNIT_ASSERT(ruby->random() == 2200);
    CPPUNIT_ASSERT(ruby->random() == 8047);
    ruby->setBounds(1234, 9876);
    ruby->seed(6789);
    CPPUNIT_ASSERT(ruby->random() == 4081);
    CPPUNIT_ASSERT(ruby->random() == 2200);
    CPPUNIT_ASSERT(ruby->random() == 8047);
    delete ruby;
}
