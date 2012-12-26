#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class ClassListBuilder : public AST::NodeVisitor
		{
		protected:

			Output & mOut;
			ProgramInfo * mProgram;

		public:

			INLINE ClassListBuilder(Output & out, ProgramInfo * program) :  mOut(out), mProgram(program) { }
			
			virtual void visit(AST::ClassDefinitionNode * classDefinition);
		};
	}
}