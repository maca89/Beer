#include "stdafx.h"

#include "IdentifierNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void IdentifierNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}