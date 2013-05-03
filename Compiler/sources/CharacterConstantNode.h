#pragma once

#include "prereq.h"
#include "ConstantNode.h"

namespace Beer
{
	namespace AST
	{
		class CharacterConstantNode : public ConstantNode
		{
		protected:

			char mValue;

		public:

			INLINE CharacterConstantNode(char value)
				:	mValue(value)
			{ }


			INLINE char getValue() const
			{
				return mValue;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}