#include "stdafx.h"

#include "StringConstantNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void StringConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}