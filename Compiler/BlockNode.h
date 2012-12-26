#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class BlockNode : public Node
		{
		protected:

			Statements * mStatements;

		public:

			BlockNode();
			BlockNode(StatementNode * statement);
			INLINE BlockNode(Statements * statements) : mStatements(statements) { }
			virtual ~BlockNode();

			INLINE Statements * getStatements() { return mStatements; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}