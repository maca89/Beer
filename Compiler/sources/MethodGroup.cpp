#include "stdafx.h"

#include "MethodGroup.h"

#include "NameBuilder.h"
#include "Method.h"
#include "SemanticException.h"
#include "StringDescriptor.h"

using namespace Beer::SemanticAnalysis;

Method * MethodGroup::getMethod(const std::string & qualifiedName)
{
	MethodMap::iterator it = mMethods.find(qualifiedName);

	if (it == mMethods.end()) return NULL;

	return it->second;
}

void MethodGroup::addMethod(Method * method)
{
	std::string qualName = NameBuilder::buildQualifiedName(method->getName()->getValue(), method->getParameters());

	if (hasMethod(qualName)) throw SemanticException("Method " + method->getFullName()->getValue() + " already exists");

	mMethods[qualName] = method;

	if (method->hasFlags(Method::Abstract)) incAbstractMethods();
}

void MethodGroup::removeMethod(Method * method)
{
	std::string qualName = NameBuilder::buildQualifiedName(method->getName()->getValue(), method->getParameters());

	ASSERT(hasMethod(qualName), "Method " + method->getFullName()->getValue() + " does not exist");

	mMethods.erase(qualName);

	if (method->hasFlags(Method::Abstract)) decAbstractMethods();
}