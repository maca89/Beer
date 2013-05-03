#include "stdafx.h"

#include "Method.h"

#include "Block.h"
#include "Class.h"
#include "MethodParameter.h"
#include "Output.h"
#include "Program.h"
#include "StringDescriptor.h"

using namespace Beer::SemanticAnalysis;

Block * Method::setBlock(::Beer::AST::BlockNode * block)
{
	mBlock = new Block(block, mParameterId);

	mDeclarator->getProgram()->getDescriptorDictionary()->addDescriptor(mBlock);

	return mBlock;
}

MethodParameter::id Method::getParameterCount() const
{
	return mParameters.size();
}

MethodParameter * Method::getParameter(const std::string & name)
{
	for (MethodParameterList::iterator it = mParameters.begin(); it != mParameters.end(); it++)
	{
		if ((*it)->getName()->getValue() == name) return *it;
	}

	return NULL;
}

bool Method::hasParameter(const std::string & name) const
{
	for (MethodParameterList::const_iterator it = mParameters.begin(); it != mParameters.end(); it++)
	{
		if ((*it)->getName()->getValue() == name) return true;
	}

	return false;
}

MethodParameter * Method::addParameter(const std::string & type, const std::string & name, ::Beer::Output & out)
{
	DescriptorDictionary * dict = mDeclarator->getProgram()->getDescriptorDictionary();

	if (hasParameter(name))
	{
		out.addError("Multiple parameters " + name + " of method " + mName->getValue() + " class " + mDeclarator->getName()->getValue());

		return NULL;
	}

	MethodParameter * param = new MethodParameter(
		--mParameterId,
		MethodParameter::NoFlag,
		mDeclarator->getProgram()->getClass(type),
		dict->addStringDescriptor(name));

	dict->addDescriptor(param);

	mParameters.push_back(param);

	return param;
}

MethodParameter * Method::addReturnParameter(const std::string & type, ::Beer::Output & out)
{
	DescriptorDictionary * dict = mDeclarator->getProgram()->getDescriptorDictionary();

	MethodParameter * param = new MethodParameter(
		--mParameterId,
		MethodParameter::NoFlag,
		mDeclarator->getProgram()->getClass(type),
		dict->addStringDescriptor(type));

	dict->addDescriptor(param);

	mReturnParameters.push_back(param);

	return param;
}