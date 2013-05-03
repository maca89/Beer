#include "stdafx.h"

#include "BlockNode.h"

#include "StatementNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

typedef StatementList::iterator StatementListIterator;

BlockNode::BlockNode()
{
	mStatements = new StatementList;
}

BlockNode::BlockNode(StatementNode * statement)
{
	mStatements = new StatementList;

	mStatements->push_back(statement);
}

BlockNode::~BlockNode()
{
	for (StatementListIterator it = mStatements->begin(); it != mStatements->end(); it++)
	{
		delete *it;
	}

	delete mStatements;
}

void BlockNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}