#include "stdafx.h"

void BeerlangCompiler::AST::IntegerConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit((ConstantNode *)this);
	visitor->visit(this);
}