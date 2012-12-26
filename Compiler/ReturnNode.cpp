#include "stdafx.h"

BeerlangCompiler::AST::ReturnNode::~ReturnNode()
{
	for (ExpressionListIterator it = mArguments->begin(); it != mArguments->end(); it++)
	{
		delete *it;
	}

	delete mArguments;
}

void BeerlangCompiler::AST::ReturnNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}