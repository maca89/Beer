#include "stdafx.h"

BeerlangCompiler::AST::WhileNode::~WhileNode()
{
	delete mCondition;

	delete mBody;
}

void BeerlangCompiler::AST::WhileNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}