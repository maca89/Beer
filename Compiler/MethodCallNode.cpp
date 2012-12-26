#include "stdafx.h"

BeerlangCompiler::AST::MethodCallNode::MethodCallNode(ExpressionNode * instance, std::string name)
	: mInstance(instance), mName(name), mType(Expression)
{
	mArguments = new ExpressionList;
}

BeerlangCompiler::AST::MethodCallNode::MethodCallNode(ExpressionNode * instance, std::string name, ExpressionNode * argument)
	 : mInstance(instance), mName(name), mType(Expression)
{
	mArguments = new ExpressionList;

	mArguments->push_back(argument);
}

BeerlangCompiler::AST::MethodCallNode::~MethodCallNode()
{
	delete mInstance;

	for (ExpressionListIterator it = mArguments->begin(); it != mArguments->end(); it++)
	{
		delete *it;
	}

	delete mArguments;
}

void BeerlangCompiler::AST::MethodCallNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}