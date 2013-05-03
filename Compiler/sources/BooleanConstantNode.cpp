#include "stdafx.h"

#include "BooleanConstantNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void BooleanConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}