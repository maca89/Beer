#include "stdafx.h"

void BeerlangCompiler::AST::StringConstantNode::accept(NodeVisitor * visitor)
{
	visitor->visit((ConstantNode *)this);
	visitor->visit(this);
}