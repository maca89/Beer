#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class StatementNode;

		typedef std::list<StatementNode *> StatementList;

		class BlockNode : public Node
		{
		protected:

			StatementList * mStatements;

		public:

			BlockNode();
			BlockNode(StatementNode * statement);

			INLINE BlockNode(StatementList * statements)
				:	mStatements(statements)
			{ }


			virtual ~BlockNode();


			INLINE StatementList * getStatements()
			{
				return mStatements;
			}


			virtual void accept(NodeVisitor * visitor);

		};
	}
}