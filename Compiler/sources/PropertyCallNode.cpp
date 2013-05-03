#include "stdafx.h"

#include "PropertyCallNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

PropertyCallNode::~PropertyCallNode()
{
	delete mInstance;
}

void PropertyCallNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}