#include "stdafx.h"

#include "ClassListBuilder.h"

#include "Class.h"
#include "ClassDefinitionNode.h"
#include "Program.h"

using namespace Beer::SemanticAnalysis;

void ClassListBuilder::visit(::Beer::AST::ClassDefinitionNode * classDefinition)
{
	Class::flags flags = Class::NoFlag;

	if (classDefinition->isNative()) flags |= Class::Native;

	mProgram->addClass(classDefinition->getName(), flags, mOut);
}