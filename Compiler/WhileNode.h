#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class WhileNode : public StatementNode
		{
		protected:

			ExpressionNode * mCondition;
			BlockNode * mBody;

		public:

			INLINE WhileNode(ExpressionNode * condition, BlockNode * body) : mCondition(condition), mBody(body) { }
			virtual ~WhileNode();

			INLINE ExpressionNode * getCondition() { return mCondition; }
			INLINE BlockNode * getBody() { return mBody; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}