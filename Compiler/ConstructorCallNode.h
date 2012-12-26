#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class ConstructorCallNode : public ExpressionNode
		{
		protected:

			std::string mName;
			ExpressionList * mArguments;

		public:

			INLINE ConstructorCallNode(std::string name, ExpressionList * arguments) : mName(name), mArguments(arguments) { }
			virtual ~ConstructorCallNode();

			INLINE std::string getName() { return mName; }
			INLINE ExpressionList * getArguments() { return mArguments; }

			virtual void accept(NodeVisitor * visitor);
		};
	}
}