#pragma once

#include "prereq.h"
#include "NodeVisitor.h"

namespace Beer
{
	namespace AST
	{
		class PrinterVisitor : public NodeVisitor
		{

		protected:

			std::string prefix;

		public:

			virtual void visit(IdentifierNode * identifier);
			virtual void visit(ThisNode * thisNode);

			virtual void visit(PropertyCallNode * call);
			virtual void visit(MethodCallNode * call);
			virtual void visit(ConstructorCallNode * call);

			virtual void visit(BooleanConstantNode * constant);
			virtual void visit(CharacterConstantNode * constant);
			virtual void visit(IntegerConstantNode * constant);
			virtual void visit(FloatConstantNode * constant);
			virtual void visit(StringConstantNode * constant);

			virtual void visit(MethodParameterNode * parameter);
			virtual void visit(MethodDefinitionNode * method);

			virtual void visit(BlockNode * block);

			virtual void visit(VariableDefinitionNode * definition);
			virtual void visit(VariableDeclarationNode * declaration);
			virtual void visit(AssignmentNode * assignment);
			virtual void visit(IfNode * ifNode);
			virtual void visit(WhileNode * whileNode);
			virtual void visit(ReturnNode * returnNode);


			virtual void visit(MemberDeclarationNode * member);
			
			virtual void visit(DataSectionNode * section);
			virtual void visit(EventsSectionNode * section);
			virtual void visit(InterfaceSectionNode * section);
			virtual void visit(ProtectedSectionNode * section);

			virtual void visit(ClassDefinitionNode * definition);

			virtual void visit(ProgramNode * program);

		protected:

			void extendPrefix();
			void shortenPrefix();

		};
	}
}