#include "stdafx.h"

#include "DataSectionNode.h"

#include "NodeVisitor.h"

using namespace Beer::AST;

DataSectionNode::~DataSectionNode()
{
	delete mMembers;
}

void DataSectionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}