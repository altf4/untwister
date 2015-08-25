/*
    Copyright Bishop Fox, moloch 2014

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

#include <cppunit/ui/text/TestRunner.h>
#include <iostream>

#include "../ConsoleColors.h"
#include "TestUntwister.h"
#include "TestPRNGFactory.h"
#include "TestMt19937.h"
#include "TestRuby.h"
#include "Test_PHP_mt19937.h"
#include "TestJava.h"

bool executeAllTests()
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(TestMt19937::suite());
    runner.addTest(Test_PHP_Mt19937::suite());
    runner.addTest(TestRuby::suite());
    runner.addTest(TestJava::suite());
    runner.addTest(TestPRNGFactory::suite());
    runner.addTest(TestUntwister::suite());
    return runner.run();
}

int main(int argc, char *argv[])
{
    std::cout << INFO << "Executing all unit tests, please wait...";
    std::cout.flush();
    if (!executeAllTests())
    {
        std::cout << WARN << "One or more tests failed!" << std::endl;
    }
    return 0;
}
