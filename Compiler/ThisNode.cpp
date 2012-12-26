#include "stdafx.h"

void BeerlangCompiler::AST::ThisNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}