#include "stdafx.h"

#include "AssignmentNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void AssignmentNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}