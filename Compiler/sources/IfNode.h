#pragma once

#include "prereq.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		class BlockNode;
		class ExpressionNode;

		typedef std::map<Beer::AST::ExpressionNode *, Beer::AST::BlockNode *> ExpressionBlockMap;

		class IfNode : public StatementNode
		{
		protected:

			ExpressionBlockMap * mIfBlocks;
			BlockNode * mElseBlock;

		public:

			IfNode(ExpressionNode * condition, BlockNode * block);

			virtual ~IfNode();


			INLINE ExpressionBlockMap * getIfBlocks()
			{
				return mIfBlocks;
			}

			INLINE BlockNode * getElseBlock()
			{
				return mElseBlock;
			}


			void addIfBlock(ExpressionNode * condition, BlockNode * block);
			
			INLINE void addElseBlock(BlockNode * block)
			{
				mElseBlock = block;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}