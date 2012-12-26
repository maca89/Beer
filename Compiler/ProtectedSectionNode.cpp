#include "stdafx.h"

void BeerlangCompiler::AST::ProtectedSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit((SectionNode *)this);
	visitor->visit(this);
}