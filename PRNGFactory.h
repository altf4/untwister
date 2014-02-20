/*
 * PRNGFactory.h
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#ifndef PRNGFACTORY_H_
#define PRNGFACTORY_H_

#include <string>
#include <map>

#include "prngs/PRNG.h"
#include "prngs/Mt19937.h"
#include "prngs/GlibcRand.h"

/* Template to bind constructor to mapped string */
template<typename T> PRNG* create() { return new T; }
typedef std::map<std::string, PRNG* (*)()> PRNGLibrary;

class PRNGFactory
{
public:
    PRNGFactory();
    virtual ~PRNGFactory();

    PRNG* getInstance(std::string);
    std::vector<const std::string> getNames(void);

private:
    PRNGLibrary library;
};

#endif /* PRNGFACTORY_H_ */
