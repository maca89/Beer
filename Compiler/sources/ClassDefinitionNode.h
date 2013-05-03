#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class SectionNode;

		typedef std::list<SectionNode *> SectionList;

		class ClassDefinitionNode : public Node
		{
		protected:

			bool			mNative;
			std::string		mName;
			SectionList *	mSections;

		public:

			INLINE ClassDefinitionNode(bool native, const std::string & name, SectionList * sections)
				:	mNative(native),
					mName(name),
					mSections(sections)
			{ }

			INLINE ClassDefinitionNode(const std::string & name, SectionList * sections)
				:	mNative(false),
					mName(name),
					mSections(sections)
			{ }


			virtual ~ClassDefinitionNode();


			INLINE bool isNative() const
			{
				return mNative;
			}

			INLINE const std::string & getName() const
			{
				return mName;
			}

			INLINE SectionList * getSections()
			{
				return mSections;
			}
			

			virtual void accept(NodeVisitor * visitor);
		};
	}
}