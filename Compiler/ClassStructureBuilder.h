#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class ClassStructureBuilder : public AST::NodeVisitor
		{
		protected:

			Output & mOut;
			ProgramInfo * mProgram;

			ClassInfo * mClass;

		public:

			INLINE ClassStructureBuilder(Output & out, ProgramInfo * program) :  mOut(out), mProgram(program), mClass(NULL) { }
			
			virtual void visit(AST::ClassDefinitionNode * classDefinition);
			virtual void visit(AST::MemberDeclarationNode * member);
			virtual void visit(AST::MethodDefinitionNode * method);
		};
	}
}