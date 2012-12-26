#include "stdafx.h"

void BeerlangCompiler::AST::CallOperatorNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}