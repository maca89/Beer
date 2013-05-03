#include "stdafx.h"

#include "ProtectedSectionNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void ProtectedSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}