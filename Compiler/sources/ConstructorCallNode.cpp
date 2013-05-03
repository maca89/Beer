#include "stdafx.h"

#include "ConstructorCallNode.h"

#include "ExpressionNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

ConstructorCallNode::~ConstructorCallNode()
{
	for (ExpressionList::iterator it = mArguments->begin(); it != mArguments->end(); it++)
	{
		delete *it;
	}

	delete mArguments;
}

void ConstructorCallNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}