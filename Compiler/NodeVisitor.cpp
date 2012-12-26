#include "stdafx.h"

/*void BeerlangCompiler::AST::NodeVisitor::visit(ConstantNode * constant)
{
}*/

/*void BeerlangCompiler::AST::NodeVisitor::visit(ExpressionNode * expression)
{
}*/

void BeerlangCompiler::AST::NodeVisitor::visit(IdentifierNode * identifier)
{
}

void BeerlangCompiler::AST::NodeVisitor::visit(ThisNode * thisNode)
{
}

/*void BeerlangCompiler::AST::NodeVisitor::visit(UnaryOperatorNode * unaryOperator)
{
unaryOperator->getOperand()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(BinaryOperatorNode * binaryOperator)
{
binaryOperator->getLeft()->accept(this);
binaryOperator->getRight()->accept(this);
}*/

void BeerlangCompiler::AST::NodeVisitor::visit(PropertyCallNode * call)
{
	call->getInstance()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(MethodCallNode * call)
{
	if (call->getInstance()) call->getInstance()->accept(this);

	for (ExpressionListIterator it = call->getArguments()->begin(); it != call->getArguments()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(ConstructorCallNode * call)
{
	for (ExpressionListIterator it = call->getArguments()->begin(); it != call->getArguments()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(BooleanConstantNode * constant)
{
}

void BeerlangCompiler::AST::NodeVisitor::visit(CharacterConstantNode * constant)
{	
}

void BeerlangCompiler::AST::NodeVisitor::visit(IntegerConstantNode * constant)
{	
}

void BeerlangCompiler::AST::NodeVisitor::visit(FloatConstantNode * constant)
{	
}

void BeerlangCompiler::AST::NodeVisitor::visit(StringConstantNode * constant)
{	
}

void BeerlangCompiler::AST::NodeVisitor::visit(MethodParameterNode * parameter)
{
	if (parameter->getValue()) parameter->getValue()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(MethodDefinitionNode * method)
{
	for (MethodParameterListIterator it = method->getParameters()->begin(); it != method->getParameters()->end(); it++)
	{
		(*it)->accept(this);
	}

	if (method->getBody())
	{
		method->getBody()->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(BlockNode * block)
{
	for (StatementsIterator it = block->getStatements()->begin(); it != block->getStatements()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(VariableDeclarationNode * declaration)
{
}

void BeerlangCompiler::AST::NodeVisitor::visit(VariableDefinitionNode * definition)
{
	definition->getDeclaration()->accept(this);
	definition->getExpression()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(AssignmentNode * assignment)
{
	assignment->getLValue()->accept(this);
	assignment->getExpression()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(IfNode * ifNode)
{
	for (ExpressionBlockMapIterator it = ifNode->getIfBlocks()->begin(); it != ifNode->getIfBlocks()->end(); it++)
	{
		it->first->accept(this);
		it->second->accept(this);
	}

	if (ifNode->getElseBlock()) ifNode->getElseBlock()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(WhileNode * whileNode)
{
	whileNode->getCondition()->accept(this);
	whileNode->getBody()->accept(this);
}

void BeerlangCompiler::AST::NodeVisitor::visit(ReturnNode * returnNode)
{
	for (ExpressionListIterator it = returnNode->getArguments()->begin(); it != returnNode->getArguments()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(MemberDeclarationNode * member)
{

}

void BeerlangCompiler::AST::NodeVisitor::visit(DataSectionNode * section)
{
	for (MemberDeclarationListIterator it = section->getMembers()->begin(); it != section->getMembers()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(EventsSectionNode * section)
{
}

void BeerlangCompiler::AST::NodeVisitor::visit(InterfaceSectionNode * section)
{
	for (MethodDefinitionListIterator it = section->getDefinitions()->begin(); it != section->getDefinitions()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(ProtectedSectionNode * section)
{
}

void BeerlangCompiler::AST::NodeVisitor::visit(ClassDefinitionNode * definition)
{
	for (SectionsIterator it = definition->getSections()->begin(); it != definition->getSections()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void BeerlangCompiler::AST::NodeVisitor::visit(ProgramNode * program)
{
	for (ClassDefinitionListIterator it = program->getClasses()->begin(); it != program->getClasses()->end(); it++)
	{
		(*it)->accept(this);
	}
}