#include "stdafx.h"

#include "CharacterConstantNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void CharacterConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}