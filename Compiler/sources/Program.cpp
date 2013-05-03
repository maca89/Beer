#include "stdafx.h"

#include "Program.h"

#include "Output.h"

using namespace Beer::SemanticAnalysis;

Program::Program()
	: mContainsEP(true)
{
}

Program::~Program()
{
}

Class * Program::getClass(std::string name)
{
	ClassMap::iterator it = mClassMap.find(name);

	if (it == mClassMap.end()) return NULL;

	return it->second;
}

/*BeerCompiler::SemanticAnalysis::Class * BeerCompiler::SemanticAnalysis::Program::addExternalClass(std::string name, class_flags flags, Output & out)
{
	Class * cls = getClass(name);

	if (cls)
	{	
		out.addError("Mutiple classes with name " + name);

		return NULL;
	}

	cls = new Class(this, mDictionary.addStringDescriptor(name), flags);

	mExternalClasses.push_back(cls);
	mClassMap[name] = cls;

	return cls;
}*/

Class * Program::addClass(std::string name, Class::flags flags, ::Beer::Output & out)
{
	Class * cls = getClass(name);

	if (cls)
	{	
		out.addError("Mutiple classes with name " + name);

		return NULL;
	}

	bool native = (flags & Class::Native) != 0;

	cls = new Class(this, mDictionary.addStringDescriptor(name), flags);

	if (native)
	{
		mExternalClasses.push_back(cls);
	}
	else
	{
		mClasses.push_back(cls);	
		mDictionary.addDescriptor(cls);
	}
	
	mClassMap[name] = cls;

	return cls;
}
