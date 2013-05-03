#pragma once

#include "prereq.h"
#include "ConstantNode.h"

namespace Beer
{
	namespace AST
	{
		class FloatConstantNode : public ConstantNode
		{
		protected:

			double mValue;

		public:

			INLINE FloatConstantNode(double value)
				:	mValue(value)
			{ }


			INLINE double getValue() const
			{
				return mValue;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}