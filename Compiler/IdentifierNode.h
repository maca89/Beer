#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class IdentifierNode : public LValueNode
		{
		protected:

			std::string mName;

		public:

			INLINE IdentifierNode(std::string name) : mName(name) { }

			INLINE std::string getName() const { return mName; }

			virtual void accept(NodeVisitor * visitor);
		};

		typedef std::list<IdentifierNode *> IdentifierList;
		typedef std::list<IdentifierNode *>::iterator IdentifierListIterator;
	}
}