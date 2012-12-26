#include "stdafx.h"

BeerlangCompiler::SemanticAnalysis::StringConstant * BeerlangCompiler::SemanticAnalysis::DescriptorDictionary::getStringDescriptor(std::string value)
{
	StringConstantMapIterator it = mStringDescriptors.find(value);

	if (it == mStringDescriptors.end()) return NULL;

	return it->second;
}

BeerlangCompiler::SemanticAnalysis::StringConstant * BeerlangCompiler::SemanticAnalysis::DescriptorDictionary::addStringConstant(std::string value)
{
	StringConstantMapIterator it = mStringDescriptors.find(value);

	if (it != mStringDescriptors.end())
	{
		it->second->incrementUsed();
		return it->second;
	}

	StringConstant * string = new StringConstant(value);

	string->incrementUsed();

	addDescriptor(string);
	mStringDescriptors[value] = string;

	return string;
}