#include "stdafx.h"

void BeerlangCompiler::AST::MemberDeclarationNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}