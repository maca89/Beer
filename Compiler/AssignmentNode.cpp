#include "stdafx.h"

void BeerlangCompiler::AST::AssignmentNode::accept(NodeVisitor * visitor)
{
	visitor->visit((AssignmentNode *)this);
}