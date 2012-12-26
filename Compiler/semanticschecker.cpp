#include "stdafx.h"

/*void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::check(AST::ProgramNode * program)
{
mOut.addInfo("Checking semantics of program");

NodeVisitor::visit(program);
}*/

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::ClassDefinitionNode * definiton)
{
	mClass = mProgram->getClass(definiton->getName());

	if (mClass)
	{
		mOut.addInfo("Checking semantics of class " + definiton->getName());

		NodeVisitor::visit(definiton);
	}

	mClass = NULL;
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::MemberDeclarationNode * member)
{
	MemberInfo * memberInfo = mClass->getMember(member->getName());

	if (memberInfo)
	{
		mOut.addInfo("Checking semantics of member " + memberInfo->getFullName());

		if (memberInfo->getType())
		{
			memberInfo->getType()->isUsed(true);
		}
		else
		{
			mOut.addError("Class " + member->getType() + " doesn't exist");	
		}
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::MethodDefinitionNode * method)
{
	mMethod = mClass->getMethod(mClass->createMethodFullName(method->getName(), method->getParameters()));

	if (mMethod)
	{
		mOut.addInfo("Checking semantics of method " + mMethod->getFullName()->getValue());

		for (AST::StringListIterator it = method->getReturnParameters()->begin(); it != method->getReturnParameters()->end(); it++)
		{
			ClassInfo * type = mProgram->getClass(*it);

			if (type)
			{
				type->isUsed(true);
			}
			else
			{
				mOut.addError("Class " + (*it) + " doesn't exist");
			}
		}

		NodeVisitor::visit(method);

		if (!mMethod->hasFlags(MethodInfo::Native))
		{
			if (!mMethod->isTerminated())
			{
				if (mMethod->getType() == MethodInfo::Constructor)
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
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::MethodParameterNode * methodParameter)
{
	ClassInfo * type = mProgram->getClass(methodParameter->getType());

	if (type)
	{
		type->isUsed(true);
	}
	else
	{
		mOut.addError("Class " + methodParameter->getType() + " doesn't exist");
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::BlockNode * block)
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

	for (AST::StatementsIterator it = block->getStatements()->begin(); it != block->getStatements()->end(); it++)
	{
		(*it)->accept(this);

		if (terminated != mBlock->isTerminated() && (*it) != block->getStatements()->back())
		{
			terminated = true;

			mOut.addWarning("Unreachable code in method " + mMethod->getFullName()->getValue());
		}
	}

	ASSERT (mBlock->getStackSize() == 0, "Stack size is not zero");

	mBlock = mBlock->getParent();
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::VariableDefinitionNode * variable)
{
	mOut.addInfo("Checking semantics of variable declaration " + variable->getDeclaration()->getName());

	variable->getExpression()->accept(this);

	ClassInfo * declarationType = mProgram->getClass(variable->getDeclaration()->getType());

	if (declarationType)
	{
		declarationType->isUsed(true);

		ExpressionInfo * expressionType = mBlock->getExpression(variable->getExpression());

		if (expressionType && expressionType->getType())
		{
			if (declarationType != expressionType->getType())
			{
				mOut.addError("Expression of type " + expressionType->getType()->getName()->getValue() + " cannot be assigned to variable " + variable->getDeclaration()->getName() + " of type " + variable->getDeclaration()->getType());
			}
			else
			{
				mBlock->declareVariable(declarationType, variable->getDeclaration()->getName(), mOut);
			}
		}
	}
	else
	{
		mOut.addError("Class " + variable->getDeclaration()->getType() + " doesn't exist");	
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::IfNode * ifNode)
{
	mOut.addInfo("Checking semantics of if");

	NodeVisitor::visit(ifNode);

	bool terminated = true;

	for (ExpressionBlockMapIterator it = ifNode->getIfBlocks()->begin(); it != ifNode->getIfBlocks()->end(); it++)
	{
		ExpressionInfo * condition = mBlock->getExpression(it->first);

		if (condition)
		{
			if (condition->getType()->getName()->getValue() != "Boolean")
			{
				mOut.addError("Condition of if must be Boolean but is " + condition->getType()->getName()->getValue());
			}
		}

		BlockInfo * block = mBlock->getBlock(it->second);

		if (block)
		{
			terminated &= block->isTerminated();
		}
	}

	if (ifNode->getElseBlock())
	{
		BlockInfo * block = mBlock->getBlock(ifNode->getElseBlock());

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

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::WhileNode * whileNode)
{
	mOut.addInfo("Checking semantics of while");

	NodeVisitor::visit(whileNode);

	ExpressionInfo * condition = mBlock->getExpression(whileNode->getCondition());

	if (condition)
	{
		if (condition->getType()->getName()->getValue() != "Boolean")
		{
			mOut.addError("Condition of while must be Boolean but is " + condition->getType()->getName()->getValue());
		}
	}

	BlockInfo * block = mBlock->getBlock(whileNode->getBody());

	if (block)
	{
		if (block->isTerminated()) terminateBlock();
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::MethodCallNode * call)
{
	mOut.addInfo("Checking semantics of method call " + call->getName());

	if (!call->getInstance()) call->setInstance(new AST::ThisNode);

	NodeVisitor::visit(call);

	ExpressionInfo * instance = mBlock->getExpression(call->getInstance());

	if (instance && instance->getType())
	{
		instance->getType()->isUsed(true);

		std::string fullName = instance->getType()->createMethodFullName(call->getName(), call->getArguments(), mBlock);

		MethodInfo * method = instance->getType()->getMethod(fullName);

		if (method)
		{
			method->setName(mProgram->getDescriptorDictionary()->addStringConstant(method->getName()->getValue()));
			method->setFullName(mProgram->getDescriptorDictionary()->addStringConstant(method->getFullName()->getValue()));

			MethodParameterList * returnParameters = method->getReturnParameters();

			mMethod->addMethodCall(call, method);	

			if (returnParameters->size() == 0)
			{
				// Method does not return any parameters
			}
			else if (returnParameters->size() == 1)
			{
				mBlock->addExpression(call, new ExpressionInfo(returnParameters->front()->getType()));

			}
			else
			{
				mOut.addError("Multiple return parameters are not yet implemented");
			}
		}
		else
		{
			mOut.addError("Method " + fullName + " doesn't exist");
		}
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::ReturnNode * returnNode)
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
		ExpressionInfo * argumentType = mBlock->getExpression(returnNode->getArguments()->front());
		ClassInfo * returnType = returnParameters->front()->getType();

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
		// Multiple return
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::VariableDeclarationNode * variable)
{
	ClassInfo * type = mProgram->getClass(variable->getType());

	if (type)
	{
		type->isUsed(true);

		//mBlock->incrementStack();
		mBlock->declareVariable(type, variable->getName(), mOut);
	}
	else
	{
		mOut.addError("Class " + variable->getType() + " doesn't exist");
	}	
}


void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::AssignmentNode * assignment)
{
	mOut.addInfo("Checking semantics of assignment");

	NodeVisitor::visit(assignment);

	ExpressionInfo * lValueType = mBlock->getExpression(assignment->getLValue());
	ExpressionInfo * expressionType = mBlock->getExpression(assignment->getExpression());

	if (lValueType && expressionType)
	{
		if (lValueType->getType() != expressionType->getType())
		{
			mOut.addError("Expression of type " + expressionType->getType()->getName()->getValue() + " cannot be assigned to variable to " + lValueType->getType()->getName()->getValue());
		}
	}

	//mBlock->decrementStack(2);
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::BooleanConstantNode * constant)
{
	mBlock->addExpression(constant, new ExpressionInfo(mProgram->getClass("Boolean")));
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::CharacterConstantNode * constant)
{
	mBlock->addExpression(constant, new ExpressionInfo(mProgram->getClass("Character")));
}			

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::IntegerConstantNode * constant)
{
	mBlock->addExpression(constant, new ExpressionInfo(mProgram->getClass("Integer")));
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::FloatConstantNode * constant)
{
	mBlock->addExpression(constant, new ExpressionInfo(mProgram->getClass("Float")));
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::StringConstantNode * constant)
{
	mBlock->addExpression(constant, new ExpressionInfo(mProgram->getClass("String")));

	mProgram->getDescriptorDictionary()->addStringConstant(constant->getValue());
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::ConstructorCallNode * call)
{
	mOut.addInfo("Checking semantics of constructor call " + call->getName());

	//mBlock->incrementStack();

	NodeVisitor::visit(call);

	ClassInfo * type = mProgram->getClass(call->getName());

	if (type)
	{
		std::string fullName = type->createMethodFullName(type->getName()->getValue(), call->getArguments(), mBlock);
	
		type->isUsed(true);

		MethodInfo * constructor = type->getMethod(fullName);

		if (!constructor)
		{
			mOut.addError("There is no method " + fullName);
		}
		else
		{
			constructor->setFullName(mProgram->getDescriptorDictionary()->addStringConstant(constructor->getFullName()->getValue()));

			//mBlock->decrementStack(constructor->getParameters()->size());
			mBlock->addExpression(call, new ExpressionInfo(type));
			mMethod->addMethodCall(call, constructor);
		}
	}
	else
	{
		mOut.addError("Class " + call->getName() + " doesn't exist");
	}
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::ThisNode * thisNode)
{
	//mOut.addInfo("Checking semantics of this");

	//mBlock->incrementStack();
	mBlock->addExpression(thisNode, new ExpressionInfo(0, ExpressionInfo::Parameter, mClass));
}

void BeerlangCompiler::SemanticAnalysis::SemanticsChecker::visit(AST::IdentifierNode * identifier)
{
	//mOut.addInfo("Checking semantics of identifier " + identifier->getName());

	VariableInfo * variable = mBlock->getVariable(identifier->getName());

	if (!variable)
	{
		MethodParameterInfo * parameter = mMethod->getParameter(identifier->getName());

		if (!parameter)
		{
			MemberInfo * member = mClass->getMember(identifier->getName());

			if (!member)
			{
				mOut.addError(identifier->getName() + " is not local variable nor parameter nor member variable");
			}	
			else
			{
				//mBlock->incrementStack();
				mBlock->addExpression(identifier, new ExpressionInfo(member->getId(), ExpressionInfo::Member, member->getType()));
			}
		}
		else
		{
			//mBlock->incrementStack();
			mBlock->addExpression(identifier, new ExpressionInfo(parameter->getId(), ExpressionInfo::Parameter, parameter->getType()));
		}
	}
	else
	{
		//mBlock->incrementStack();
		mBlock->addExpression(identifier, new ExpressionInfo(variable->getId(), ExpressionInfo::Variable, variable->getType()));
	}
}