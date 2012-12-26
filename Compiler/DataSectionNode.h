#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class DataSectionNode : public SectionNode
		{
		protected:

			MemberDeclarationList * mMembers;

		public:

			INLINE DataSectionNode(MemberDeclarationList * members) : mMembers(members) { }
			virtual ~DataSectionNode();

			INLINE MemberDeclarationList * getMembers() { return mMembers; }

			virtual void accept(NodeVisitor * visitor);
		};
	}
}