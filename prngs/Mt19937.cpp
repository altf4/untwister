/*
 * Mt19937.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: moloch
 */

#include "Mt19937.h"

Mt19937::Mt19937()
{
	seedValue = generator.default_seed;
}

Mt19937::~Mt19937() {}

const std::string Mt19937::getName()
{
	return "mt19937";
}

void Mt19937::seed(uint32_t value)
{
	seedValue = value;
	this->generator.seed(value);
}

uint32_t Mt19937::getSeed()
{
	return seedValue;
}

uint32_t Mt19937::random(void)
{
	return this->generator();
}
