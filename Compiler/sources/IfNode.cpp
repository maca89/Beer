#include "stdafx.h"

#include "IfNode.h"

#include "BlockNode.h"
#include "ExpressionNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

IfNode::IfNode(ExpressionNode * condition, BlockNode * block)
	:	mElseBlock(NULL)
{
	mIfBlocks = new std::map<ExpressionNode *, BlockNode *>;

	mIfBlocks->insert(std::pair<ExpressionNode *, BlockNode *>(condition, block));
}

IfNode::~IfNode()
{
	for (std::map<ExpressionNode *, BlockNode *>::iterator it = mIfBlocks->begin(); it != mIfBlocks->end(); it++)
	{
		delete it->first;
		delete it->second;
	}

	delete mIfBlocks;

	if (mElseBlock) delete mElseBlock;
}

void IfNode::addIfBlock(ExpressionNode * condition, BlockNode * block)
{
	mIfBlocks->insert(std::pair<ExpressionNode *, BlockNode *>(condition, block));
}

void IfNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}