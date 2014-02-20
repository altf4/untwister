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
}

PRNGFactory::~PRNGFactory() {}

PRNG* PRNGFactory::getInstance(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    return library[name]();
}

std::vector<const std::string> PRNGFactory::getNames()
{
    PRNGLibrary::iterator iter;
    std::vector<const std::string> names;
    for (iter = library.begin(); iter != library.end(); ++iter)
    {
        names.push_back(iter->first);
    }
    return names;
}
