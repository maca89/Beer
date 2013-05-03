#pragma once

#include "prereq.h"
#include "ConstantNode.h"

namespace Beer
{
	namespace AST
	{
		class StringConstantNode : public ConstantNode
		{
		protected:

			std::string mValue;

		public:

			INLINE StringConstantNode(const std::string & value)
				:	mValue(value)
			{ }


			INLINE const std::string & getValue() const
			{
				return mValue;
			}


			virtual void accept(NodeVisitor * visitor);

		};
	}
}