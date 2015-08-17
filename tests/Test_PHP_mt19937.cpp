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
