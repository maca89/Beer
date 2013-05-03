#pragma once

#include "prereq.h"
#include "SectionNode.h"

namespace Beer
{
	namespace AST
	{
		class MethodDefinitionNode;

		typedef std::list<MethodDefinitionNode *> MethodDefinitionList;

		class InterfaceSectionNode : public SectionNode
		{
		protected:

			std::string mName;
			MethodDefinitionList * mDefinitions;	

		public:

			INLINE InterfaceSectionNode(MethodDefinitionList * definitions)
				:	mName(),
					mDefinitions(definitions)
			{ }


			INLINE InterfaceSectionNode(const std::string & name, MethodDefinitionList * definitions)
				:	mName(name),
					mDefinitions(definitions)
			{ }


			virtual ~InterfaceSectionNode();


			INLINE const std::string & getName() const
			{
				return mName;
			}

			INLINE MethodDefinitionList * getDefinitions()
			{
				return mDefinitions;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}