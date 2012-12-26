#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class InterfaceSectionNode : public SectionNode
		{
		protected:

			std::string mName;
			MethodDefinitionList * mDefinitions;			

		public:

			INLINE InterfaceSectionNode(MethodDefinitionList * definitions)
				: mName(), mDefinitions(definitions) { }

			INLINE InterfaceSectionNode(std::string name, MethodDefinitionList * definitions)
				: mName(name), mDefinitions(definitions) { }

			virtual ~InterfaceSectionNode();

			INLINE std::string getName() const { return mName; }
			INLINE MethodDefinitionList * getDefinitions() { return mDefinitions; }

			virtual void accept(NodeVisitor * visitor);
		};
	}
}