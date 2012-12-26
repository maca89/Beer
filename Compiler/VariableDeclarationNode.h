#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class VariableDeclarationNode : public StatementNode
		{
		protected:

			std::string mType;
			std::string mName;

		public:

			INLINE VariableDeclarationNode(std::string type, std::string name) : mType(type), mName(name) { }

			INLINE std::string getType() const { return mType; }	
			INLINE std::string getName() const { return mName; }

			virtual void accept(NodeVisitor * visitor);
		};

		typedef std::list<VariableDeclarationNode *> VariableDeclarationList;
		typedef std::list<VariableDeclarationNode *>::iterator VariableDeclarationListIterator;
	}
}

