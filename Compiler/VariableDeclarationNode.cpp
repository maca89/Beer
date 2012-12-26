#include "stdafx.h"

void BeerlangCompiler::AST::VariableDeclarationNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}