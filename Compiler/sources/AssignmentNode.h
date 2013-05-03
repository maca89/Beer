#pragma once

#include "prereq.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		class LValueNode;
		class ExpressionNode;

		class AssignmentNode : public StatementNode
		{
		protected:

			LValueNode * mLValue;
			ExpressionNode * mExpression;

		public:

			INLINE AssignmentNode(LValueNode * lValue, ExpressionNode * expression)
				:	mLValue(lValue),
				mExpression(expression)
			{ }


			INLINE LValueNode * getLValue()
			{
				return mLValue;
			}

			INLINE ExpressionNode * getExpression()
			{
				return mExpression;
			}


			virtual void accept(NodeVisitor * visitor);

		};
	}
}