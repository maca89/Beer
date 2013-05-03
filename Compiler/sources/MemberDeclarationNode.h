#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class MemberDeclarationNode : Node
		{
		protected:

			std::string mType;
			std::string mName;

		public:

			INLINE MemberDeclarationNode(const std::string & type, const std::string & name)
				:	mType(type),
					mName(name)
			{ }


			INLINE const std::string & getType() const
			{
				return mType;
			}

			INLINE const std::string & getName() const
			{
				return mName;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}

