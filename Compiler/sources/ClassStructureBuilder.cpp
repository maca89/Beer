#include "stdafx.h"

#include "ClassStructureBuilder.h"

#include "Class.h"
#include "ClassDefinitionNode.h"
#include "InterfaceSectionNode.h"
#include "MemberDeclarationNode.h"
#include "MethodDefinitionNode.h"
#include "Output.h"
#include "Program.h"
#include "StringDescriptor.h"

using namespace Beer::SemanticAnalysis;

void ClassStructureBuilder::visit(::Beer::AST::ProgramNode * program)
{
	NodeVisitor::visit(program);

	if (!mProgram->containsEntryPoint()) mOut.addError("There must be at least one entry point (EntryPoint::run())");
}

void ClassStructureBuilder::visit(::Beer::AST::ClassDefinitionNode * classDefinition)
{
	mClass = mProgram->getClass(classDefinition->getName());

	ASSERT(mClass, "Class is null");

	if (mClass)
	{
		NodeVisitor::visit(classDefinition);
	}

	mClass = NULL;
}

void ClassStructureBuilder::visit(::Beer::AST::InterfaceSectionNode * section)
{
	if (!section->getName().empty())
	{
		Class * parent = mProgram->getClass(section->getName());

		if (parent)
		{
			if (parent->hasAncestor(mClass))
			{
				mOut.addError(parent->getName()->getValue() + " cannot be parent of " + mClass->getName()->getValue() + " because " + mClass->getName()->getValue() + " is ancestor of " + parent->getName()->getValue());
			}
			else
			{
				mClass->addParent(parent, mOut);

				if (parent->getName()->getValue() == "EntryPoint")
				{
					if (mClass->getMethod("run()")) mProgram->containsEntryPoint(true);
				}
			}
		}
		else
		{
			mOut.addError("There is no class " + section->getName());
		}
	}

	NodeVisitor::visit(section);
}

void ClassStructureBuilder::visit(::Beer::AST::MemberDeclarationNode * member)
{
	mClass->addMember(Member::NoFlags, member->getType(), member->getName(), mOut);
}

void ClassStructureBuilder::visit(::Beer::AST::MethodDefinitionNode * method)
{
	Method::flags flags = Method::NoFlag;

	if (!method->getBody()) flags |= (mClass->hasFlags(Class::Native) ? Method::Native : Method::Abstract); // Native class cannot have abstract methods

	if (method->getReturnParameters()->empty())
	{
		if (method->getName() == mClass->getName()->getValue())
		{
			flags |= Method::Special;
			mClass->addConstructor(flags, method->getParameters(), mOut);
		}
		else if (mProgram->getClass(method->getName()))
		{
			mClass->addConversion(flags, method->getName(), mOut);
		}
		else
		{
			mClass->addMethod(flags, method->getName(), method->getParameters(), mOut);
		}
	}
	else
	{
		mClass->addMethod(flags, method->getReturnParameters(), method->getName(), method->getParameters(), mOut);
	}
}