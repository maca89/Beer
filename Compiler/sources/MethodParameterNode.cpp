#include "stdafx.h"

#include "MethodParameterNode.h"

#include "ConstantNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

MethodParameterNode::~MethodParameterNode()
{
	if (mValue) delete mValue;
}

void MethodParameterNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}