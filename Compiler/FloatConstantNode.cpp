#include "stdafx.h"

void BeerlangCompiler::AST::FloatConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit((ConstantNode *)this);
	visitor->visit(this);
}