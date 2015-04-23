/*
 * PRNGFactory.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "PRNGFactory.h"

PRNGFactory::PRNGFactory()
{
    library[GLIBC_RAND] = &create<GlibcRand>;
    library[MT19937] = &create<Mt19937>;
    library[RUBY_RAND] = &create<Ruby>;
    library[PHP_MT_RAND] = &create<PHP_mt19937>;
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
