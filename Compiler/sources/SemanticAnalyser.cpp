#include "stdafx.h"

#include "SemanticAnalyser.h"

#include "Program.h"
#include "ProgramNode.h"

using namespace Beer::SemanticAnalysis;

void SemanticAnalyser::analyse(::Beer::AST::ProgramNode * program)
{
	program->accept(&mClb);
	program->accept(&mCsb);
	program->accept(&mSc);
	
	Class::id id = 0;

	ClassList * classes = mProgram->getExternalClasses();	
	
	for (ClassList::iterator it = classes->begin(); it != classes->end(); it++)
	{
		(*it)->setId(id++);
	}

	classes = mProgram->getClasses();

	for (ClassList::iterator it = classes->begin(); it != classes->end(); it++)
	{
		(*it)->setId(id++);
	}
}

