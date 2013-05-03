#pragma once

#include "prereq.h"
#include "ConstantNode.h"

namespace Beer
{
	namespace AST
	{
		class BooleanConstantNode : public ConstantNode
		{
		protected:

			bool mValue;

		public:

			INLINE BooleanConstantNode(bool value)
				:	mValue(value)
			{ }


			INLINE bool getValue() const
			{
				return mValue;
			}

			
			virtual void accept(NodeVisitor * visitor);

		};
	}
}