#include "stdafx.h"

void BeerlangCompiler::AST::CharacterConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit((ConstantNode *)this);
	visitor->visit(this);
}