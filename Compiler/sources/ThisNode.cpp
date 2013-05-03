#include "stdafx.h"

#include "ThisNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void ThisNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}