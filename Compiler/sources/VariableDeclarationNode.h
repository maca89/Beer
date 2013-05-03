#pragma once

#include "prereq.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		class VariableDeclarationNode : public StatementNode
		{
		protected:

			std::string mType;
			std::string mName;

		public:

			INLINE VariableDeclarationNode(const std::string & type, const std::string & name)
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

