#include "stdafx.h"

void BeerlangCompiler::AST::PropertyDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}