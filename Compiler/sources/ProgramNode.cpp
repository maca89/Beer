#include "stdafx.h"

#include "ProgramNode.h"

#include "ClassDefinitionNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

ProgramNode::ProgramNode()
{
	mClasses = new ClassDefinitionList;
}

ProgramNode::~ProgramNode()
{
	delete mClasses;
}

void ProgramNode::addClasses(ClassDefinitionList * classes)
{
	for (ClassDefinitionList::iterator it = classes->begin(); it != classes->end(); it++)
	{
		mClasses->push_back(*it);
	}
}

void ProgramNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}