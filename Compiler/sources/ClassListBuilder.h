#pragma once

#include "prereq.h"
#include "NodeVisitor.h"

namespace Beer
{
	class Output;

	namespace SemanticAnalysis
	{
		class Program;

		/*
		 * This class is used for creating a list of available classes in program.
		 */
		class ClassListBuilder : public AST::NodeVisitor
		{
		protected:

			Output &	mOut;
			Program *	mProgram;

		public:

			INLINE ClassListBuilder(Output & out, Program * program)
				:	mOut(out),
					mProgram(program)
			{ }
			

			/*
			 * This method traverses all class definition and foreach adds new class to program.
			 */
			virtual void visit(AST::ClassDefinitionNode * classDefinition);
		};
	}
}