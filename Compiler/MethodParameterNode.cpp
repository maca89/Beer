#include "stdafx.h"

BeerlangCompiler::AST::MethodParameterNode::~MethodParameterNode()
{
	if (mValue) delete mValue;
}

void BeerlangCompiler::AST::MethodParameterNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}