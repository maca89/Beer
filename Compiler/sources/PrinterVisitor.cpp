#include "stdafx.h"

#include "PrinterVisitor.h"

#include "IdentifierNode.h"
#include "PropertyCallNode.h"
#include "MethodCallNode.h"
#include "ConstructorCallNode.h"
#include "BooleanConstantNode.h"
#include "CharacterConstantNode.h"
#include "IntegerConstantNode.h"
#include "FloatConstantNode.h"
#include "StringConstantNode.h"
#include "MethodParameterNode.h"
#include "ConstantNode.h"
#include "MethodDefinitionNode.h"
#include "BlockNode.h"
#include "VariableDefinitionNode.h"
#include "VariableDeclarationNode.h"
#include "AssignmentNode.h"
#include "LValueNode.h"
#include "IfNode.h"
#include "WhileNode.h"
#include "ReturnNode.h"
#include "DataSectionNode.h"
#include "MemberDeclarationNode.h"
#include "InterfaceSectionNode.h"
#include "ClassDefinitionNode.h"
#include "ProgramNode.h"

using namespace Beer::AST;

void PrinterVisitor::visit(IdentifierNode * identifier)
{
	std::cout << prefix << "Identifier " << identifier->getName() << std::endl;
}

void PrinterVisitor::visit(ThisNode * thisNode)
{
	std::cout << prefix << "This " << std::endl;
}

void PrinterVisitor::visit(PropertyCallNode * call)
{
	std::cout << prefix << "Property call " << call->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(call);

	shortenPrefix();
}

void PrinterVisitor::visit(MethodCallNode * call)
{
	std::cout << prefix << "Method call " << call->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(call);

	shortenPrefix();
}

void PrinterVisitor::visit(ConstructorCallNode * call)
{
	std::cout << prefix << "Constructor call " << call->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(call);

	shortenPrefix();
}

void PrinterVisitor::visit(BooleanConstantNode * constant)
{
	std::cout << prefix << "Boolean: " << (constant->getValue() ? "true" : "false") << std::endl;
}

void PrinterVisitor::visit(CharacterConstantNode * constant)
{
	std::cout << prefix << "Character: " << constant->getValue() << std::endl;
}

void PrinterVisitor::visit(IntegerConstantNode * constant)
{
	std::cout << prefix << "Integer: " << constant->getValue() << std::endl;
}

void PrinterVisitor::visit(FloatConstantNode * constant)
{
	std::cout << prefix << "Floating: " << std::fixed << constant->getValue() << std::endl;
}

void PrinterVisitor::visit(StringConstantNode * constant)
{
	std::cout << prefix << "String: " << std::fixed << constant->getValue() << std::endl;
}

void PrinterVisitor::visit(MethodParameterNode * parameter)
{
	std::cout << prefix << "Parameter " << parameter->getType() << " " << parameter->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(parameter);

	shortenPrefix();
}

void PrinterVisitor::visit(MethodDefinitionNode * method)
{
	std::cout << prefix << "Method " << method->getName() << std::endl;

	extendPrefix();

	for (AST::StringList::iterator it = method->getReturnParameters()->begin(); it != method->getReturnParameters()->end(); it++)
	{
		std::cout << prefix << "Return parameter " << *it << std::endl;
	}

	NodeVisitor::visit(method);

	shortenPrefix();
}

void PrinterVisitor::visit(AssignmentNode * assignment)
{
	std::cout << prefix << "Assignment" << std::endl;

	extendPrefix();

	NodeVisitor::visit(assignment);

	shortenPrefix();
}

void PrinterVisitor::visit(IfNode * ifNode)
{
	std::cout << prefix << "If " << std::endl;

	extendPrefix();

	NodeVisitor::visit(ifNode);

	shortenPrefix();
}

void PrinterVisitor::visit(WhileNode * whileNode)
{
	std::cout << prefix << "While" << std::endl;

	extendPrefix();

	NodeVisitor::visit(whileNode);

	shortenPrefix();
}

void PrinterVisitor::visit(ReturnNode * returnNode)
{
	std::cout << prefix << "Return " << std::endl;

	extendPrefix();

	NodeVisitor::visit(returnNode);

	shortenPrefix();
}

void PrinterVisitor::visit(BlockNode * block)
{
	std::cout << prefix << "Block" << std::endl;

	extendPrefix();

	NodeVisitor::visit(block);

	shortenPrefix();
}

void PrinterVisitor::visit(VariableDeclarationNode * declaration)
{
	std::cout << prefix << "Variable declaration: " << declaration->getType() << " " << declaration->getName() << std::endl;
}

void PrinterVisitor::visit(MemberDeclarationNode * member)
{
	std::cout << prefix << "Member declaration: " << member->getType() << " " << member->getName() << std::endl;
}

void PrinterVisitor::visit(DataSectionNode * section)
{
	std::cout << prefix << "Data" << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void PrinterVisitor::visit(EventsSectionNode * section)
{
	std::cout << prefix << "Events" << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void PrinterVisitor::visit(InterfaceSectionNode * section)
{
	std::cout << prefix << "Interface " << section->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void PrinterVisitor::visit(ProtectedSectionNode * section)
{
	std::cout << prefix << "Protected" << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void PrinterVisitor::visit(VariableDefinitionNode * definition)
{
	std::cout << prefix << "Variable definition" << std::endl;

	extendPrefix();

	NodeVisitor::visit(definition);

	shortenPrefix();
}

void PrinterVisitor::visit(ClassDefinitionNode * definition)
{
	std::cout << prefix << "Class " << definition->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(definition);

	shortenPrefix();
}

void PrinterVisitor::visit(ProgramNode * program)
{
	std::cout << prefix << "Program" << std::endl;

	extendPrefix();

	NodeVisitor::visit(program);

	shortenPrefix();
}

void PrinterVisitor::extendPrefix()
{
	prefix += "    ";
}

void PrinterVisitor::shortenPrefix()
{
	prefix = prefix.substr(0, prefix.length() - 4);
}