#include "stdafx.h"

BeerlangCompiler::AST::PropertyCallNode::~PropertyCallNode()
{
	delete mInstance;
}

void BeerlangCompiler::AST::PropertyCallNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}