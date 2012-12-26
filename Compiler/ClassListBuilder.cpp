#include "stdafx.h"

void BeerlangCompiler::SemanticAnalysis::ClassListBuilder::visit(AST::ClassDefinitionNode * classDefinition)
{
	class_flags flags = ClassInfo::NoFlag;

	if (classDefinition->isNative()) flags |= ClassInfo::Native;

	mProgram->addClass(classDefinition->getName(), flags, mOut);
}