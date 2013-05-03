#pragma once

#include "prereq.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		class ExpressionNode;

		typedef std::list<ExpressionNode *> ExpressionList;

		class ReturnNode : public StatementNode
		{
		protected:

			ExpressionList * mArguments;

		public:

			INLINE ReturnNode(ExpressionList * arguments)
				:	mArguments(arguments)
			{ }


			virtual ~ReturnNode();


			INLINE ExpressionList * getArguments()
			{
				return mArguments;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}