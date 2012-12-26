#include "stdafx.h"

BeerlangCompiler::AST::DataSectionNode::~DataSectionNode()
{
	delete mMembers;
}

void BeerlangCompiler::AST::DataSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit((SectionNode *)this);
	visitor->visit(this);
}