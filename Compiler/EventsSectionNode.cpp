#include "stdafx.h"

void BeerlangCompiler::AST::EventsSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit((SectionNode *)this);
	visitor->visit(this);
}