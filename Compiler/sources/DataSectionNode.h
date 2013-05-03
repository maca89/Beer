#pragma once

#include "prereq.h"
#include "SectionNode.h"

namespace Beer
{
	namespace AST
	{
		class MemberDeclarationNode;

		typedef std::list<MemberDeclarationNode *> MemberDeclarationList;

		class DataSectionNode : public SectionNode
		{
		protected:

			MemberDeclarationList * mMembers;

		public:

			INLINE DataSectionNode(MemberDeclarationList * members)
				: mMembers(members)
			{ }


			virtual ~DataSectionNode();


			INLINE MemberDeclarationList * getMembers()
			{
				return mMembers;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}