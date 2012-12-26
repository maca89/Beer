#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class ReturnNode : public StatementNode
		{
		protected:

			ExpressionList * mArguments;

		public:

			INLINE ReturnNode(ExpressionList * arguments) : mArguments(arguments) { }
			virtual ~ReturnNode();

			INLINE ExpressionList * getArguments() { return mArguments; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}