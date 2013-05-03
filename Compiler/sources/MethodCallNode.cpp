#include "stdafx.h"

#include "MethodCallNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

MethodCallNode::MethodCallNode(ExpressionNode * instance, const std::string & name)
	:	mInstance(instance),
		mName(name),
		mType(Expression)
{
	mArguments = new ExpressionList;
}

MethodCallNode::MethodCallNode(ExpressionNode * instance, const std::string & name, ExpressionNode * argument)
	:	mInstance(instance),
		mName(name),
		mType(Expression)
{
	mArguments = new ExpressionList;

	mArguments->push_back(argument);
}

MethodCallNode::~MethodCallNode()
{
	delete mInstance;

	for (ExpressionList::iterator it = mArguments->begin(); it != mArguments->end(); it++)
	{
		delete *it;
	}

	delete mArguments;
}

void MethodCallNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}