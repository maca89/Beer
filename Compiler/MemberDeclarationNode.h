#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class MemberDeclarationNode : Node
		{
		protected:

			std::string mType;
			std::string mName;

		public:

			INLINE MemberDeclarationNode(std::string type, std::string name)
				: mType(type), mName(name) { }

			INLINE std::string getType() const { return mType; }	
			INLINE std::string getName() const { return mName; }

			virtual void accept(NodeVisitor * visitor);
		};

		typedef std::list<BeerlangCompiler::AST::MemberDeclarationNode *> MemberDeclarationList;
		typedef std::list<BeerlangCompiler::AST::MemberDeclarationNode *>::iterator MemberDeclarationListIterator;
	}
}

