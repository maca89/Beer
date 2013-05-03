#include "stdafx.h"

#include "FloatConstantNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void FloatConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}