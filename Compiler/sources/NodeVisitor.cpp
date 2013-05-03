#include "stdafx.h"

#include "NodeVisitor.h"

#include "AssignmentNode.h"
#include "BlockNode.h"
#include "ClassDefinitionNode.h"
#include "ConstantNode.h"
#include "ConstructorCallNode.h"
#include "DataSectionNode.h"
#include "IfNode.h"
#include "InterfaceSectionNode.h"
#include "LValueNode.h"
#include "MemberDeclarationNode.h"
#include "MethodCallNode.h"
#include "MethodDefinitionNode.h"
#include "MethodParameterNode.h"
#include "ProgramNode.h"
#include "PropertyCallNode.h"
#include "ReturnNode.h"
#include "VariableDefinitionNode.h"
#include "VariableDeclarationNode.h"
#include "WhileNode.h"

using namespace Beer::AST;

void NodeVisitor::visit(IdentifierNode * identifier)
{
}

void NodeVisitor::visit(ThisNode * thisNode)
{
}

void NodeVisitor::visit(PropertyCallNode * call)
{
	call->getInstance()->accept(this);
}

void NodeVisitor::visit(MethodCallNode * call)
{
	if (call->getInstance()) call->getInstance()->accept(this);

	for (ExpressionList::iterator it = call->getArguments()->begin(); it != call->getArguments()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(ConstructorCallNode * call)
{
	for (ExpressionList::iterator it = call->getArguments()->begin(); it != call->getArguments()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(BooleanConstantNode * constant)
{
}

void NodeVisitor::visit(CharacterConstantNode * constant)
{	
}

void NodeVisitor::visit(IntegerConstantNode * constant)
{	
}

void NodeVisitor::visit(FloatConstantNode * constant)
{	
}

void NodeVisitor::visit(StringConstantNode * constant)
{	
}

void NodeVisitor::visit(MethodParameterNode * parameter)
{
	if (parameter->getValue()) parameter->getValue()->accept(this);
}

void NodeVisitor::visit(MethodDefinitionNode * method)
{
	for (MethodParameterList::iterator it = method->getParameters()->begin(); it != method->getParameters()->end(); it++)
	{
		(*it)->accept(this);
	}

	if (method->getBody())
	{
		method->getBody()->accept(this);
	}
}

void NodeVisitor::visit(BlockNode * block)
{
	for (StatementList::iterator it = block->getStatements()->begin(); it != block->getStatements()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(VariableDeclarationNode * declaration)
{
}

void NodeVisitor::visit(VariableDefinitionNode * definition)
{
	definition->getDeclaration()->accept(this);
	definition->getExpression()->accept(this);
}

void NodeVisitor::visit(AssignmentNode * assignment)
{
	assignment->getLValue()->accept(this);
	assignment->getExpression()->accept(this);
}

void NodeVisitor::visit(IfNode * ifNode)
{
	for (ExpressionBlockMap::iterator it = ifNode->getIfBlocks()->begin(); it != ifNode->getIfBlocks()->end(); it++)
	{
		it->first->accept(this);
		it->second->accept(this);
	}

	if (ifNode->getElseBlock()) ifNode->getElseBlock()->accept(this);
}

void NodeVisitor::visit(WhileNode * whileNode)
{
	whileNode->getCondition()->accept(this);
	whileNode->getBody()->accept(this);
}

void NodeVisitor::visit(ReturnNode * returnNode)
{
	for (ExpressionList::iterator it = returnNode->getArguments()->begin(); it != returnNode->getArguments()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(MemberDeclarationNode * member)
{

}

void NodeVisitor::visit(DataSectionNode * section)
{
	for (MemberDeclarationList::iterator it = section->getMembers()->begin(); it != section->getMembers()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(EventsSectionNode * section)
{
}

void NodeVisitor::visit(InterfaceSectionNode * section)
{
	for (MethodDefinitionList::iterator it = section->getDefinitions()->begin(); it != section->getDefinitions()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(ProtectedSectionNode * section)
{
}

void NodeVisitor::visit(ClassDefinitionNode * definition)
{
	for (SectionList::iterator it = definition->getSections()->begin(); it != definition->getSections()->end(); it++)
	{
		(*it)->accept(this);
	}
}

void NodeVisitor::visit(ProgramNode * program)
{
	for (ClassDefinitionList::iterator it = program->getClasses()->begin(); it != program->getClasses()->end(); it++)
	{
		(*it)->accept(this);
	}
}