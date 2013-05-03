#pragma once

#include "prereq.h"
#include "ExpressionNode.h"

namespace Beer
{
	namespace AST
	{
		typedef std::list<ExpressionNode *> ExpressionList;

		class ConstructorCallNode : public ExpressionNode
		{
		protected:

			std::string mName;
			ExpressionList * mArguments;

		public:

			INLINE ConstructorCallNode(const std::string & name, ExpressionList * arguments)
				:	mName(name),
					mArguments(arguments)
			{ }


			virtual ~ConstructorCallNode();


			INLINE const std::string & getName()
			{
				return mName;
			}

			INLINE ExpressionList * getArguments()
			{
				return mArguments;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}