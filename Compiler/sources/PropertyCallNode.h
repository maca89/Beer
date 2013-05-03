#pragma once

#include "prereq.h"
#include "ExpressionNode.h"

namespace Beer
{
	namespace AST
	{
		class PropertyCallNode : public ExpressionNode
		{
		protected:

			ExpressionNode * mInstance;
			std::string mName;

		public:

			INLINE PropertyCallNode(ExpressionNode * instance, const std::string & name)
				:	mInstance(instance),
					mName(name)
			{ }


			virtual ~PropertyCallNode();


			INLINE ExpressionNode * getInstance()
			{
				return mInstance;
			}

			INLINE const std::string & getName() const
			{
				return mName;
			}


			virtual void accept(NodeVisitor * visitor);

		};
	}
}