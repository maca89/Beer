#include "stdafx.h"

#include "WhileNode.h"

#include "ExpressionNode.h"
#include "BlockNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

WhileNode::~WhileNode()
{
	delete mCondition;

	delete mBody;
}

void WhileNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}