#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class CallOperatorNode : public BinaryOperatorNode
		{
		public:

			INLINE CallOperatorNode(ExpressionNode * left, ExpressionNode * right) : BinaryOperatorNode(left, ".", right) { }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}