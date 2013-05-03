#include "stdafx.h"

#include "NameBuilder.h"

#include "Block.h"
#include "Class.h"
#include "Expression.h"
#include "MethodParameterNode.h"
#include "StringDescriptor.h"

using namespace Beer::SemanticAnalysis;

std::string NameBuilder::buildFullName(const std::string & className, const std::string & methodName, ::Beer::AST::MethodParameterList * parameters)
{
	return className + "::" + buildQualifiedName(methodName, parameters);
}

std::string NameBuilder::buildFullName(const std::string & className, const std::string & methodName, MethodParameterList * parameters)
{
	return className + "::" + buildQualifiedName(methodName, parameters);
}

std::string NameBuilder::buildFullName(const std::string & className, const std::string & methodName, ::Beer::AST::ExpressionList * arguments, Block * block)
{
	return className + "::" + buildQualifiedName(methodName, arguments, block);
}

std::string NameBuilder::buildQualifiedName(const std::string & methodName, ::Beer::AST::MethodParameterList * parameters)
{
	std::string qualName = methodName + "(";

	if (parameters)
	{
		AST::MethodParameterList::iterator it = parameters->begin();

		if (it != parameters->end())
		{
			qualName += (*it)->getType();
			it++;
		}

		for (; it != parameters->end(); it++)
		{
			qualName += "," + (*it)->getType(); 
		}
	}

	qualName += ")";

	return qualName;
}

std::string NameBuilder::buildQualifiedName(const std::string & methodName, MethodParameterList * parameters)
{
	std::string qualName = methodName + "(";

	if (parameters)
	{
		MethodParameterList::iterator it = parameters->begin();

		if (it != parameters->end())
		{
			Class * parameterType = (*it)->getType();

			ASSERT(parameterType, "Parameter type is NULL");

			qualName += parameterType->getName()->getValue();
			it++;
		}

		for (; it != parameters->end(); it++)
		{
			Class * parameterType = (*it)->getType();

			ASSERT(parameterType, "Parameter type is NULL");

			qualName += "," + parameterType->getName()->getValue();
		}
	}

	qualName += ")";

	return qualName;
}

std::string NameBuilder::buildQualifiedName(const std::string & methodName, ::Beer::AST::ExpressionList * arguments, Block * block)
{
	std::string qualName = methodName + "(";

	Expression * type = NULL;

	AST::ExpressionList::iterator it = arguments->begin();

	if (it != arguments->end())
	{
		type = block->getExpression(*it);

		if (type && type->getType())
		{
			qualName += type->getType()->getName()->getValue();
		}

		it++;
	}

	for (; it != arguments->end(); it++)
	{
		type = block->getExpression(*it);

		if (type && type->getType())
		{
			qualName += "," + type->getType()->getName()->getValue();
		}
	}

	qualName += ")";

	return qualName;
}