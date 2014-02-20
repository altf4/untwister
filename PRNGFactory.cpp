/*
 * PRNGFactory.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "PRNGFactory.h"

PRNGFactory::PRNGFactory()
{
	library["mt19937"] = &create<Mt19937>;
	library["glibc-rand"] = &create<GlibcRand>;
}

PRNGFactory::~PRNGFactory() {}

PRNG* PRNGFactory::getInstance(std::string name)
{
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	return library[name]();
}
