#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class SemanticsChecker : public AST::NodeVisitor
		{
		protected:

			Output & mOut;
			ProgramInfo * mProgram;

			ClassInfo * mClass;
			MethodInfo * mMethod;
			BlockInfo * mBlock;

		public:

			INLINE SemanticsChecker(Output & out, ProgramInfo * program)
				: mOut(out), mProgram(program), mClass(NULL), mMethod(NULL), mBlock(NULL) { }

			//void check(AST::ProgramNode * program);

				//void prepare(AST::ProgramNode * program);

			virtual void visit(AST::ClassDefinitionNode * definiton);

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
