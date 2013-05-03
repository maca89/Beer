#include "stdafx.h"

#include "ReturnNode.h"

#include "ExpressionNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

ReturnNode::~ReturnNode()
{
	for (ExpressionList::iterator it = mArguments->begin(); it != mArguments->end(); it++)
	{
		delete *it;
	}

	delete mArguments;
}

void ReturnNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}