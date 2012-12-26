#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class ProgramNode : public ExpressionNode
		{
		protected:

			ClassDefinitionList * mClasses;

		public:

			ProgramNode();
			virtual ~ProgramNode();

			INLINE ClassDefinitionList * getClasses() { return mClasses; }

			void addClasses(ClassDefinitionList * classes);

			virtual void accept(NodeVisitor * visitor);
		};
	}
}