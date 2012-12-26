#include "stdafx.h"

BeerlangCompiler::AST::ConstructorCallNode::~ConstructorCallNode()
{
	for (ExpressionListIterator it = mArguments->begin(); it != mArguments->end(); it++)
	{
		delete *it;
	}

	delete mArguments;
}

void BeerlangCompiler::AST::ConstructorCallNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}