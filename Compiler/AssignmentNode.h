#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class AssignmentNode : public StatementNode
		{
		protected:

			LValueNode * mLValue;
			ExpressionNode * mExpression;

		public:

			INLINE AssignmentNode(LValueNode * lValue, ExpressionNode * expression) : mLValue(lValue), mExpression(expression) { }

			INLINE LValueNode * getLValue() { return mLValue; }
			INLINE ExpressionNode * getExpression() { return mExpression; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}