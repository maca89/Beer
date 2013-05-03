#pragma once

#include "prereq.h"
#include "LValuenode.h"

namespace Beer
{
	namespace AST
	{
		class IdentifierNode : public LValueNode
		{
		protected:

			std::string mName;

		public:

			INLINE IdentifierNode(const std::string & name)
				:	mName(name)
			{ }


			INLINE const std::string & getName() const
			{
				return mName;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}