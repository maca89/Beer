#include "stdafx.h"

void BeerlangCompiler::SemanticAnalysis::ClassStructureBuilder::visit(AST::ClassDefinitionNode * classDefinition)
{
	mClass = mProgram->getClass(classDefinition->getName());

	if (mClass)
	{
		NodeVisitor::visit(classDefinition);
	}

	mClass = NULL;
}

void BeerlangCompiler::SemanticAnalysis::ClassStructureBuilder::visit(AST::MemberDeclarationNode * member)
{
	mClass->addMember(MemberInfo::NoFlags, member->getType(), member->getName(), mOut);
}

void BeerlangCompiler::SemanticAnalysis::ClassStructureBuilder::visit(AST::MethodDefinitionNode * method)
{
	MethodInfo * methodInfo = NULL;
	
	if (method->getReturnParameters()->empty())
	{
		if (method->getName() == mClass->getName()->getValue())
		{
			methodInfo = mClass->addConstructor(method->getParameters(), mOut);

			/*if (!(mClass->getFlags() & ClassInfo::Native))
			{
				AST::ExpressionList * arguments = new AST::ExpressionList;

				AST::ThisNode * thisNode = new AST::ThisNode;

				arguments->push_back(thisNode);

				AST::ReturnNode * returnNode = new AST::ReturnNode(arguments);

				method->getBody()->getStatements()->push_back(returnNode);
			}*/
		}
		else if (mProgram->getClass(method->getName()))
		{
			methodInfo = mClass->addConversion(method->getName(), mOut);
		}
		else
		{
			methodInfo = mClass->addMethod(method->getName(), method->getParameters(), mOut);
		}
	}
	else
	{
		methodInfo = mClass->addMethod(method->getReturnParameters(), method->getName(), method->getParameters(), mOut);
	}

	if (methodInfo)
	{
		if (!method->getBody() && !methodInfo->hasFlags(MethodInfo::Native))
		{
			mOut.addError("Method " + methodInfo->getFullName()->getValue() + " must have body, because it is not native method");
		}
	}
}