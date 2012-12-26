#include "stdafx.h"

/*void BeerlangCompiler::AST::PrinterVisitor::visit(ExpressionNode * expression)
{
	std::cout << prefix << "-- Expression:";
}*/

void BeerlangCompiler::AST::PrinterVisitor::visit(IdentifierNode * identifier)
{
	std::cout << prefix << "Identifier " << identifier->getName() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(ThisNode * thisNode)
{
	std::cout << prefix << "This " << std::endl;
}

/*void BeerlangCompiler::AST::PrinterVisitor::visit(UnaryOperatorNode * unaryOperator)
{
	std::cout << prefix << "Unary operator" << unaryOperator->getOperator() << std::endl;

	extendPrefix();

	NodeVisitor::visit(unaryOperator);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(BinaryOperatorNode * binaryOperator)
{
	std::cout << prefix << "Binary operator: " << binaryOperator->getOperator() << std::endl;

	extendPrefix();

	NodeVisitor::visit(binaryOperator);

	shortenPrefix();
}*/

void BeerlangCompiler::AST::PrinterVisitor::visit(PropertyCallNode * call)
{
	std::cout << prefix << "Property call " << call->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(call);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(MethodCallNode * call)
{
	std::cout << prefix << "Method call " << call->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(call);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(ConstructorCallNode * call)
{
	std::cout << prefix << "Constructor call " << call->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(call);

	shortenPrefix();
}

/*void BeerlangCompiler::AST::PrinterVisitor::visit(ConstantNode * constant)
{
	std::cout << prefix << "-- Constant:";
}*/

void BeerlangCompiler::AST::PrinterVisitor::visit(BooleanConstantNode * constant)
{
	std::cout << prefix << "Boolean: " << (constant->getValue() ? "true" : "false") << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(CharacterConstantNode * constant)
{
	std::cout << prefix << "Character: " << constant->getValue() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(IntegerConstantNode * constant)
{
	std::cout << prefix << "Integer: " << constant->getValue() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(FloatConstantNode * constant)
{
	std::cout << prefix << "Floating: " << std::fixed << constant->getValue() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(StringConstantNode * constant)
{
	std::cout << prefix << "String: " << std::fixed << constant->getValue() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(MethodParameterNode * parameter)
{
	std::cout << prefix << "Parameter " << parameter->getType() << " " << parameter->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(parameter);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(MethodDefinitionNode * method)
{
	std::cout << prefix << "Method " << method->getName() << std::endl;

	extendPrefix();

	for (StringListIterator it = method->getReturnParameters()->begin(); it != method->getReturnParameters()->end(); it++)
	{
		std::cout << prefix << "Return parameter " << *it << std::endl;
	}

	NodeVisitor::visit(method);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(AssignmentNode * assignment)
{
	std::cout << prefix << "Assignment" << std::endl;

	extendPrefix();

	NodeVisitor::visit(assignment);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(IfNode * ifNode)
{
	std::cout << prefix << "If " << std::endl;

	extendPrefix();

	NodeVisitor::visit(ifNode);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(WhileNode * whileNode)
{
	std::cout << prefix << "While" << std::endl;

	extendPrefix();

	NodeVisitor::visit(whileNode);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(ReturnNode * returnNode)
{
	std::cout << prefix << "Return " << std::endl;

	extendPrefix();

	NodeVisitor::visit(returnNode);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(BlockNode * block)
{
	std::cout << prefix << "Block" << std::endl;

	extendPrefix();

	NodeVisitor::visit(block);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(VariableDeclarationNode * declaration)
{
	std::cout << prefix << "Variable declaration: " << declaration->getType() << " " << declaration->getName() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(MemberDeclarationNode * member)
{
	std::cout << prefix << "Member declaration: " << member->getType() << " " << member->getName() << std::endl;
}

void BeerlangCompiler::AST::PrinterVisitor::visit(DataSectionNode * section)
{
	std::cout << prefix << "Data" << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(EventsSectionNode * section)
{
	std::cout << prefix << "Events" << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(InterfaceSectionNode * section)
{
	std::cout << prefix << "Interface " << section->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(ProtectedSectionNode * section)
{
	std::cout << prefix << "Protected" << std::endl;

	extendPrefix();

	NodeVisitor::visit(section);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(VariableDefinitionNode * definition)
{
	std::cout << prefix << "Variable definition" << std::endl;

	extendPrefix();

	NodeVisitor::visit(definition);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(ClassDefinitionNode * definition)
{
	std::cout << prefix << "Class " << definition->getName() << std::endl;

	extendPrefix();

	NodeVisitor::visit(definition);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::visit(ProgramNode * program)
{
	std::cout << prefix << "Program" << std::endl;

	extendPrefix();

	NodeVisitor::visit(program);

	shortenPrefix();
}

void BeerlangCompiler::AST::PrinterVisitor::extendPrefix()
{
	prefix += "    ";
}

void BeerlangCompiler::AST::PrinterVisitor::shortenPrefix()
{
	prefix = prefix.substr(0, prefix.length() - 4);
}