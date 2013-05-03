#include "stdafx.h"

#include "MemberDeclarationNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void MemberDeclarationNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}