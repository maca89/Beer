#include "stdafx.h"

#include "InterfaceSectionNode.h"

#include "MethodDefinitionNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

InterfaceSectionNode::~InterfaceSectionNode()
{
	for (MethodDefinitionList::iterator it = mDefinitions->begin(); it != mDefinitions->end(); it++)
	{
		delete *it;
	}

	delete mDefinitions;
}

void InterfaceSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}