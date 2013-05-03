#pragma once

#include "prereq.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		class ExpressionNode;
		class BlockNode;

		class WhileNode : public StatementNode
		{
		protected:

			ExpressionNode * mCondition;
			BlockNode * mBody;

		public:

			INLINE WhileNode(ExpressionNode * condition, BlockNode * body)
				:	mCondition(condition),
					mBody(body)
			{ }


			virtual ~WhileNode();


			INLINE ExpressionNode * getCondition()
			{
				return mCondition;
			}

			INLINE BlockNode * getBody()
			{
				return mBody;
			}


			virtual void accept(NodeVisitor * visitor);

		};
	}
}