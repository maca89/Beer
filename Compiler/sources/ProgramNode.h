#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class ClassDefinitionNode;

		typedef std::list<ClassDefinitionNode *> ClassDefinitionList;

		class ProgramNode : public Node
		{
		protected:

			ClassDefinitionList * mClasses;

		public:

			ProgramNode();

			virtual ~ProgramNode();


			INLINE ClassDefinitionList * getClasses()
			{
				return mClasses;
			}


			void addClasses(ClassDefinitionList * classes);
			
			virtual void accept(NodeVisitor * visitor);
		};
	}
}