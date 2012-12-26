#include "stdafx.h"

BeerlangCompiler::AST::IfNode::IfNode(ExpressionNode * condition, BlockNode * block) : mElseBlock(NULL)
{
	mIfBlocks = new std::map<ExpressionNode *, BlockNode *>;

	mIfBlocks->insert(std::pair<ExpressionNode *, BlockNode *>(condition, block));
}

BeerlangCompiler::AST::IfNode::~IfNode()
{
	for (std::map<ExpressionNode *, BlockNode *>::iterator it = mIfBlocks->begin(); it != mIfBlocks->end(); it++)
	{
		delete it->first;
		delete it->second;
	}

	delete mIfBlocks;

	if (mElseBlock) delete mElseBlock;
}

void BeerlangCompiler::AST::IfNode::addIfBlock(ExpressionNode * condition, BlockNode * block)
{
	mIfBlocks->insert(std::pair<ExpressionNode *, BlockNode *>(condition, block));
}

void BeerlangCompiler::AST::IfNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}