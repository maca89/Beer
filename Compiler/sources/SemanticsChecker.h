#pragma once

#include "prereq.h"
#include "NodeVisitor.h"
#include "Method.h"
#include "Block.h"

namespace Beer
{
	class Output;

	namespace SemanticAnalysis
	{
		class Program;
		
		class SemanticsChecker : public AST::NodeVisitor
		{
		protected:

			Output & mOut;
			Program * mProgram;

			Class * mClass;
			Method * mMethod;
			Block * mBlock;
			
		public:

			INLINE SemanticsChecker(Output & out, Program * program)
				:	mOut(out),
					mProgram(program),
					mClass(NULL),
					mMethod(NULL),
					mBlock(NULL)
			{ }

			virtual void visit(AST::ClassDefinitionNode * definiton);

			virtual void visit(AST::InterfaceSectionNode * section);

			virtual void visit(AST::MemberDeclarationNode * member);

			virtual void visit(AST::MethodDefinitionNode * method);
			virtual void visit(AST::MethodParameterNode * methodParameter);

			virtual void visit(AST::BlockNode * block);

			virtual void visit(AST::VariableDefinitionNode * variable);
			virtual void visit(AST::MethodCallNode * call);

			virtual void visit(AST::IfNode * ifNode);
			virtual void visit(AST::WhileNode * whileNode);
			virtual void visit(AST::ReturnNode * returnNode);
			virtual void visit(AST::VariableDeclarationNode * variable);
			virtual void visit(AST::AssignmentNode * assignment);

			virtual void visit(AST::BooleanConstantNode * constant);
			virtual void visit(AST::CharacterConstantNode * constant);
			virtual void visit(AST::IntegerConstantNode * constant);
			virtual void visit(AST::FloatConstantNode * constant);
			virtual void visit(AST::StringConstantNode * constant);
			virtual void visit(AST::ConstructorCallNode * call);

			virtual void visit(AST::ThisNode * thisNode);
			virtual void visit(AST::IdentifierNode * identifier);

		protected:

			INLINE void terminateBlock()
			{
				mBlock->isTerminated(true);
				if (!mBlock->getParent()) mMethod->isTerminated(true);
			}
		};
	}
}
