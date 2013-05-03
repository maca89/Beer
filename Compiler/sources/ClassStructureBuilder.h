#pragma once

#include "prereq.h"
#include "NodeVisitor.h"

namespace Beer
{
	class Output;

	namespace SemanticAnalysis
	{
		class Class;
		class Program;		

		/*
		 * This class builds basic structure (members and methods) of classes in program.
		 * Usage of this class should follow after ClassListBuilder.
		 */
		class ClassStructureBuilder : public AST::NodeVisitor
		{
		protected:

			Output &	mOut;
			Program *	mProgram;

			Class *		mClass; // Currently processed class

		public:

			INLINE ClassStructureBuilder(Output & out, Program * program)
				:	mOut(out),
					mProgram(program),
					mClass(NULL)
			{ }


			virtual void visit(AST::ProgramNode * program);

			/*
			 * Retrieves class from program - supposes that it already exists.
			 */
			virtual void visit(AST::ClassDefinitionNode * classDefinition);

			/*
			 * Traverses sections of class definition.
			 */
			virtual void visit(AST::InterfaceSectionNode * section);

			/*
			 * Traverses member definitions and adds them to class.
			 */
			virtual void visit(AST::MemberDeclarationNode * member);

			/*
			 * Traverses method definitions and adds them to class.
			 */
			virtual void visit(AST::MethodDefinitionNode * method);
		};
	}
}