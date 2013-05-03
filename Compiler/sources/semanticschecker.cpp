#include "stdafx.h"

#include "SemanticsChecker.h"

#include "AssignmentNode.h"
#include "BlockNode.h"
#include "BooleanConstantNode.h"
#include "CharacterConstantNode.h"
#include "ClassDefinitionNode.h"
#include "ConstructorCallNode.h"
#include "Expression.h"
#include "FloatConstantNode.h"
#include "IdentifierNode.h"
#include "IfNode.h"
#include "IntegerConstantNode.h"
#include "MethodCallNode.h"
#include "MemberDeclarationNode.h"
#include "MethodDefinitionNode.h"
#include "MethodParameterNode.h"
#include "NameBuilder.h"
#include "Output.h"
#include "Program.h"
#include "ReturnNode.h"
#include "StringConstantNode.h"
#include "StringDescriptor.h"
#include "ThisNode.h"
#include "VariableDeclarationNode.h"
#include "VariableDefinitionNode.h"
#include "WhileNode.h"

using namespace Beer::SemanticAnalysis;

void SemanticsChecker::visit(::Beer::AST::ClassDefinitionNode * definiton)
{
	mClass = mProgram->getClass(definiton->getName());

	if (mClass)
	{
		mOut.addInfo("Checking semantics of class " + definiton->getName());

		NodeVisitor::visit(definiton);
	}

	mClass = NULL;
}

void SemanticsChecker::visit(::Beer::AST::InterfaceSectionNode * section)
{
	NodeVisitor::visit(section);
}

void SemanticsChecker::visit(::Beer::AST::MemberDeclarationNode * member)
{
	Member * mbr = mClass->getMember(member->getName());

	if (mbr)
	{
		mOut.addInfo("Checking semantics of member " + mbr->getFullName());

		if (!mbr->getType())
		{
			mOut.addError("Class " + member->getType() + " doesn't exist");	
		}
	}
}

void SemanticsChecker::visit(::Beer::AST::MethodDefinitionNode * method)
{
	mMethod = mClass->getMethod(NameBuilder::buildQualifiedName(method->getName(), method->getParameters()));

	if (mMethod)
	{
		mOut.addInfo("Checking semantics of method " + mMethod->getFullName()->getValue());

		for (AST::StringList::iterator it = method->getReturnParameters()->begin(); it != method->getReturnParameters()->end(); it++)
		{
			Class * type = mProgram->getClass(*it);

			if (!type)
			{
				mOut.addError("Class " + (*it) + " doesn't exist");
			}
		}

		NodeVisitor::visit(method);

		if (mMethod->isImplemented())
		{	
			if (!mMethod->isTerminated())
			{
				if (mMethod->getType() == Method::Constructor)
				{
					mBlock = mMethod->getBlock();

					AST::ExpressionList * arguments = new AST::ExpressionList;

					AST::ThisNode * thisNode = new AST::ThisNode;

					arguments->push_back(thisNode);

					AST::ReturnNode * returnNode = new AST::ReturnNode(arguments);

					method->getBody()->getStatements()->push_back(returnNode);

					visit(returnNode);

					mBlock = NULL;
				}
				else if (mMethod->getReturnParameters()->size() == 0)
				{
					mBlock = mMethod->getBlock();

					AST::ExpressionList * arguments = new AST::ExpressionList;

					AST::ReturnNode * returnNode = new AST::ReturnNode(arguments);

					method->getBody()->getStatements()->push_back(returnNode);

					visit(returnNode);

					mBlock = NULL;
				}
				else
				{
					mOut.addError("Method " + mMethod->getFullName()->getValue() + " is not terminated by return");
				}
			}

		}

		mMethod = NULL;
	}
}

void SemanticsChecker::visit(::Beer::AST::MethodParameterNode * methodParameter)
{
	Class * type = mProgram->getClass(methodParameter->getType());

	if (!type)
	{
		mOut.addError("Class " + methodParameter->getType() + " doesn't exist");
	}
}

void SemanticsChecker::visit(::Beer::AST::BlockNode * block)
{
	if (mBlock)
	{
		mBlock = mBlock->addBlock(block);
	}
	else
	{
		mBlock = mMethod->setBlock(block);
	}

	ASSERT(mBlock, "Block is NULL");

	bool terminated = false;

	for (AST::StatementList::iterator it = block->getStatements()->begin(); it != block->getStatements()->end(); it++)
	{
		(*it)->accept(this);

		if (terminated != mBlock->isTerminated() && (*it) != block->getStatements()->back())
		{
			terminated = true;

			mOut.addWarning("Unreachable code in method " + mMethod->getFullName()->getValue());
		}
	}

	mBlock = mBlock->getParent();
}

void SemanticsChecker::visit(::Beer::AST::VariableDefinitionNode * variable)
{
	mOut.addInfo("Checking semantics of variable declaration " + variable->getDeclaration()->getName());

	variable->getExpression()->accept(this);

	Class * declarationType = mProgram->getClass(variable->getDeclaration()->getType());

	if (declarationType)
	{
		Expression * expressionType = mBlock->getExpression(variable->getExpression());

		if (expressionType && expressionType->getType())
		{
			if (expressionType->getType()->isSubtype(declarationType))
			{
				mBlock->declareVariable(declarationType, variable->getDeclaration()->getName(), mOut);
			}
			else
			{				
				mOut.addError("Expression of type " + expressionType->getType()->getName()->getValue() + " cannot be assigned to variable " + variable->getDeclaration()->getName() + " of type " + variable->getDeclaration()->getType());
			}
		}
	}
	else
	{
		mOut.addError("Class " + variable->getDeclaration()->getType() + " doesn't exist");	
	}
}

void SemanticsChecker::visit(::Beer::AST::IfNode * ifNode)
{
	mOut.addInfo("Checking semantics of if");

	NodeVisitor::visit(ifNode);

	bool terminated = true;

	for (AST::ExpressionBlockMap::iterator it = ifNode->getIfBlocks()->begin(); it != ifNode->getIfBlocks()->end(); it++)
	{
		Expression * condition = mBlock->getExpression(it->first);

		if (condition)
		{
			if (condition->getType()->getName()->getValue() != "Boolean")
			{
				mOut.addError("Condition of if must be Boolean but is " + condition->getType()->getName()->getValue());
			}
		}

		Block * block = mBlock->getBlock(it->second);

		if (block)
		{
			terminated &= block->isTerminated();
		}
	}

	if (ifNode->getElseBlock())
	{
		Block * block = mBlock->getBlock(ifNode->getElseBlock());

		if (block)
		{
			terminated &= block->isTerminated();
		}
	}
	else
	{
		terminated = false;
	}

	if (terminated)
	{
		terminateBlock();
	}
}

void SemanticsChecker::visit(::Beer::AST::WhileNode * whileNode)
{
	mOut.addInfo("Checking semantics of while");

	NodeVisitor::visit(whileNode);

	Expression * condition = mBlock->getExpression(whileNode->getCondition());

	if (condition)
	{
		if (condition->getType()->getName()->getValue() != "Boolean")
		{
			mOut.addError("Condition of while must be Boolean but is " + condition->getType()->getName()->getValue());
		}
	}

	Block * block = mBlock->getBlock(whileNode->getBody());

	if (block)
	{
		if (block->isTerminated()) terminateBlock();
	}
}

void SemanticsChecker::visit(::Beer::AST::MethodCallNode * call)
{
	mOut.addInfo("Checking semantics of method call " + call->getName());

	if (!call->getInstance()) call->setInstance(new AST::ThisNode);

	NodeVisitor::visit(call);

	Expression * instance = mBlock->getExpression(call->getInstance());

	if (instance && instance->getType())
	{
		Method * method = instance->getType()->getMethod(NameBuilder::buildQualifiedName(call->getName(), call->getArguments(), mBlock));

		if (method)
		{
			MethodParameterList * returnParameters = method->getReturnParameters();

			mBlock->addMethodCall(call, method);

			if (returnParameters->size() == 0)
			{
				// Method does not return any parameters
			}
			else if (returnParameters->size() == 1)
			{
				mBlock->addExpression(call, new Expression(returnParameters->front()->getType()));

			}
			else
			{
				mOut.addError("Multiple return parameters are not yet implemented");
			}
		}
		else
		{
			mOut.addError("Method " + NameBuilder::buildFullName(instance->getType()->getName()->getValue(), call->getName(), call->getArguments(), mBlock) + " doesn't exist");
		}
	}
}

void SemanticsChecker::visit(::Beer::AST::ReturnNode * returnNode)
{
	mOut.addInfo("Checking semantics of return");

	NodeVisitor::visit(returnNode);

	MethodParameterList * returnParameters = mMethod->getReturnParameters();

	if (returnParameters->size() == 0)
	{
		if (returnNode->getArguments()->size() != 0)
		{
			mOut.addError("Method " + mMethod->getFullName()->getValue() + " should not return any parameters");
		}
		else
		{
			terminateBlock();
		}
	}
	else if (returnParameters->size() == 1)
	{
		Expression * argumentType = mBlock->getExpression(returnNode->getArguments()->front());
		Class * returnType = returnParameters->front()->getType();

		if (argumentType && returnType)
		{
			if (argumentType->getType() != returnType)
			{
				mOut.addError("Method should return " + returnType->getName()->getValue() + " but instead it returns " + argumentType->getType()->getName()->getValue());
			}
			else
			{
				terminateBlock();
			}
		}
	}
	else
	{
		// TODO: Multiple return
	}
}

void SemanticsChecker::visit(::Beer::AST::VariableDeclarationNode * variable)
{
	Class * type = mProgram->getClass(variable->getType());

	if (type)
	{
		mBlock->declareVariable(type, variable->getName(), mOut);
	}
	else
	{
		mOut.addError("Class " + variable->getType() + " doesn't exist");
	}	
}


void SemanticsChecker::visit(::Beer::AST::AssignmentNode * assignment)
{
	mOut.addInfo("Checking semantics of assignment");

	NodeVisitor::visit(assignment);

	Expression * lValueType = mBlock->getExpression(assignment->getLValue());
	Expression * expressionType = mBlock->getExpression(assignment->getExpression());

	if (lValueType && expressionType)
	{
		if (!expressionType->getType()->isSubtype(lValueType->getType()))
		{
			mOut.addError("Expression of type " + expressionType->getType()->getName()->getValue() + " cannot be assigned to variable of " + lValueType->getType()->getName()->getValue());
		}
	}
}

void SemanticsChecker::visit(::Beer::AST::BooleanConstantNode * constant)
{
	mBlock->addExpression(constant, new Expression(mProgram->getClass("Boolean")));
}

void SemanticsChecker::visit(::Beer::AST::CharacterConstantNode * constant)
{
	mBlock->addExpression(constant, new Expression(mProgram->getClass("Character")));
}			

void SemanticsChecker::visit(::Beer::AST::IntegerConstantNode * constant)
{
	mBlock->addExpression(constant, new Expression(mProgram->getClass("Integer")));
}

void SemanticsChecker::visit(::Beer::AST::FloatConstantNode * constant)
{
	mBlock->addExpression(constant, new Expression(mProgram->getClass("Float")));
}

void SemanticsChecker::visit(::Beer::AST::StringConstantNode * constant)
{
	mBlock->addExpression(constant, new Expression(mProgram->getClass("String")));

	mProgram->getDescriptorDictionary()->addStringDescriptor(constant->getValue());
}

void SemanticsChecker::visit(::Beer::AST::ConstructorCallNode * call)
{
	mOut.addInfo("Checking semantics of constructor call " + call->getName());


	NodeVisitor::visit(call);

	Class * type = mProgram->getClass(call->getName());

	if (type)
	{
		if (type->hasFlags(Class::Abstract))
		{
			mOut.addError("Cannot instantiate abstract class " + type->getName()->getValue());
		}
		else
		{
			std::string qualName = NameBuilder::buildQualifiedName(type->getName()->getValue(), call->getArguments(), mBlock);

			Method * ctor = type->getMethod(qualName);

			if (!ctor)
			{
				mOut.addError("There is no method " + type->getName()->getValue() + "::" + qualName);
			}
			else
			{
				mBlock->addExpression(call, new Expression(type));
				mBlock->addMethodCall(call, ctor);
			}
		}
	}
	else
	{
		mOut.addError("Class " + call->getName() + " doesn't exist");
	}
}

void SemanticsChecker::visit(::Beer::AST::ThisNode * thisNode)
{
	mBlock->addExpression(thisNode, new Expression(0, Expression::Parameter, mClass));
}

void SemanticsChecker::visit(::Beer::AST::IdentifierNode * identifier)
{
	Variable * variable = mBlock->getVariable(identifier->getName());

	if (!variable)
	{
		MethodParameter * parameter = mMethod->getParameter(identifier->getName());

		if (!parameter)
		{
			Member * member = mClass->getMember(identifier->getName());

			if (!member)
			{
				mOut.addError(identifier->getName() + " is not local variable nor parameter nor member variable");
			}	
			else
			{
				mBlock->addExpression(identifier, new Expression(member->getId(), Expression::Member, member->getType()));
			}
		}
		else
		{
			mBlock->addExpression(identifier, new Expression(parameter->getId(), Expression::Parameter, parameter->getType()));
		}
	}
	else
	{
		mBlock->addExpression(identifier, new Expression(variable->getId(), Expression::Variable, variable->getType()));
	}
}