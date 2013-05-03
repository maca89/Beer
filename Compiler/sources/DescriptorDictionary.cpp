#include "stdafx.h"

#include "DescriptorDictionary.h"

#include "SemanticException.h"
#include "StringDescriptor.h"

using namespace Beer::SemanticAnalysis;

DescriptorDictionary::~DescriptorDictionary()
{
	for (DescriptorList::iterator it = mDescriptors.begin(); it != mDescriptors.end(); it++)
	{
		delete *it;
	}
}

StringDescriptor * DescriptorDictionary::getStringDescriptor(const std::string & value)
{
	StringDescriptorMap::iterator it = mStringDescriptors.find(value);

	if (it == mStringDescriptors.end()) throw SemanticException("There is no string constant for string \"" + value + "\"");

	return it->second;
}

StringDescriptor * DescriptorDictionary::addStringDescriptor(const std::string & value)
{
	StringDescriptorMap::iterator it = mStringDescriptors.find(value);

	if (it != mStringDescriptors.end())
	{
		return it->second;
	}

	StringDescriptor * string = new StringDescriptor(value);

	addDescriptor(string);
	mStringDescriptors[value] = string;

	return string;
}

void DescriptorDictionary::removeStringDescriptor(const std::string & value)
{
	// TODO: implement
}
