#include "stdafx.h"

#include "PropertyDefinitionNode.h"

#include "BlockNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

PropertyDefinitionNode::~PropertyDefinitionNode()
{
	if (mGetter) delete mGetter;
	if (mSetter) delete mSetter;
}

void PropertyDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}