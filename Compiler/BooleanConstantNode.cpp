#include "stdafx.h"

void BeerlangCompiler::AST::BooleanConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit((ConstantNode *)this);
	visitor->visit(this);
}