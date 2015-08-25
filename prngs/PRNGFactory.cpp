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

#include "PRNGFactory.h"

PRNGFactory::PRNGFactory()
{
    library[GLIBC_RAND] = &create<GlibcRand>;
    library[MT19937] = &create<Mt19937>;
    library[RUBY_RAND] = &create<Ruby>;
    library[PHP_MT_RAND] = &create<PHP_mt19937>;
    library[JAVA] = &create<Java>;
}

PRNGFactory::~PRNGFactory() {}

PRNG* PRNGFactory::getInstance(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    return library[name]();
}

std::vector<std::string> PRNGFactory::getNames()
{
    std::vector<std::string> names;
    for (PRNGLibrary::iterator iter = library.begin(); iter != library.end(); ++iter)
    {
        names.push_back(iter->first);
    }
    return names;
}
