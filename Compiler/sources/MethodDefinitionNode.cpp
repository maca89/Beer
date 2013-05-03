#include "stdafx.h"

#include "MethodDefinitionNode.h"

#include "BlockNode.h"
#include "MethodParameterNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

MethodDefinitionNode::MethodDefinitionNode(const std::string & name, BlockNode * body)
	:	mName(name),
		mBody(body)
{
	mReturnParameters = new StringList;

	mParameters = new MethodParameterList;
}

MethodDefinitionNode::MethodDefinitionNode(const std::string & name, MethodParameterList * parameters, BlockNode * body)
	:	mName(name),
		mParameters(parameters),
		mBody(body)
{
	mReturnParameters = new StringList;
}

MethodDefinitionNode::MethodDefinitionNode(const std::string & returnParameter, const std::string & name, BlockNode * body)
	:	mName(name),
		mBody(body)
{
	mReturnParameters = new StringList;

	mReturnParameters->push_back(returnParameter);

	mParameters = new MethodParameterList;
}

MethodDefinitionNode::MethodDefinitionNode(StringList * returnParameters, const std::string & name, BlockNode * body)
	:	mReturnParameters(returnParameters),
		mName(name),
		mBody(body)
{
	mParameters = new MethodParameterList;
}

MethodDefinitionNode::MethodDefinitionNode(const std::string & returnParameter, const std::string & name, MethodParameterNode * parameter, BlockNode * body)
	:	mName(name),
		mBody(body)
{
	mReturnParameters = new StringList;

	mReturnParameters->push_back(returnParameter);

	mParameters = new MethodParameterList;

	mParameters->push_back(parameter);
}

MethodDefinitionNode::~MethodDefinitionNode()
{
	delete mReturnParameters;

	for (MethodParameterList::iterator it = mParameters->begin(); it != mParameters->end(); it++)
	{
		delete *it;
	}

	delete mParameters;

	delete mBody;
}

void MethodDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}