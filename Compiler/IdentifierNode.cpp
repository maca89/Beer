#include "stdafx.h"

void BeerlangCompiler::AST::IdentifierNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}