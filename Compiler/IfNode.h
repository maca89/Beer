#pragma once

#include "stdafx.h"

typedef std::map<BeerlangCompiler::AST::ExpressionNode *, BeerlangCompiler::AST::BlockNode *> ExpressionBlockMap;
typedef std::map<BeerlangCompiler::AST::ExpressionNode *, BeerlangCompiler::AST::BlockNode *>::iterator ExpressionBlockMapIterator;

namespace BeerlangCompiler
{
	namespace AST
	{
		class IfNode : public StatementNode
		{
		protected:

			ExpressionBlockMap * mIfBlocks;
			BlockNode * mElseBlock;

		public:

			IfNode(ExpressionNode * condition, BlockNode * block);
			virtual ~IfNode();

			INLINE ExpressionBlockMap * getIfBlocks() { return mIfBlocks; }
			INLINE BlockNode * getElseBlock() { return mElseBlock; }

			void addIfBlock(ExpressionNode * condition, BlockNode * block);
			INLINE void addElseBlock(BlockNode * block) { mElseBlock = block; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}