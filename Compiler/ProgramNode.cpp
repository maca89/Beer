#include "stdafx.h"

BeerlangCompiler::AST::ProgramNode::ProgramNode()
{
	mClasses = new ClassDefinitionList;
}

BeerlangCompiler::AST::ProgramNode::~ProgramNode()
{
	delete mClasses;
}

void BeerlangCompiler::AST::ProgramNode::addClasses(ClassDefinitionList * classes)
{
	for (ClassDefinitionListIterator it = classes->begin(); it != classes->end(); it++)
	{
		mClasses->push_back(*it);
	}
}

void BeerlangCompiler::AST::ProgramNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}