#include "stdafx.h"

BeerlangCompiler::AST::BlockNode::BlockNode()
{
	mStatements = new Statements;
}

BeerlangCompiler::AST::BlockNode::BlockNode(StatementNode * statement)
{
	mStatements = new Statements;

	mStatements->push_back(statement);
}

BeerlangCompiler::AST::BlockNode::~BlockNode()
{
	for (StatementsIterator it = mStatements->begin(); it != mStatements->end(); it++)
	{
		delete *it;
	}

	delete mStatements;
}

void BeerlangCompiler::AST::BlockNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}