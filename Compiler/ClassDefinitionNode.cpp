#include "stdafx.h"

BeerlangCompiler::AST::ClassDefinitionNode::~ClassDefinitionNode()
{
	delete mSections;
}

void BeerlangCompiler::AST::ClassDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}