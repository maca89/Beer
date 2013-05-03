#include "stdafx.h"

#include "EventsSectionNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

void EventsSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}