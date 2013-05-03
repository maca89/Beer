#include "stdafx.h"

#include "VariableDeclarationNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void VariableDeclarationNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}