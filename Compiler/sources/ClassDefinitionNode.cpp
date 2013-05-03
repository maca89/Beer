#include "stdafx.h"

#include "ClassDefinitionNode.h"

#include "NodeVisitor.h"
#include "SectionNode.h"

using namespace Beer::AST;

ClassDefinitionNode::~ClassDefinitionNode()
{
	for (SectionList::iterator it = mSections->begin(); it != mSections->end(); it++)
	{
		delete *it;
	}

	delete mSections;
}

void ClassDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}