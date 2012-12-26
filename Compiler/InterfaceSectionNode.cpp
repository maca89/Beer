#include "stdafx.h"

BeerlangCompiler::AST::InterfaceSectionNode::~InterfaceSectionNode()
{
	for (MethodDefinitionListIterator it = mDefinitions->begin(); it != mDefinitions->end(); it++)
	{
		delete *it;
	}

	delete mDefinitions;
}

void BeerlangCompiler::AST::InterfaceSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}