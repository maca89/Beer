#include "stdafx.h"

#include "IntegerConstantNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void IntegerConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}