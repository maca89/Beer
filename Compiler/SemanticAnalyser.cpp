#include "stdafx.h"

void BeerlangCompiler::SemanticAnalysis::SemanticAnalyser::analyse(AST::ProgramNode * program)
{
	program->accept(&mClb);
	program->accept(&mCsb);
	program->accept(&mSc);
	
	ClassList * classes = mProgram->getExternalClasses();
	ClassMap * classMap = mProgram->getClassMap();

	class_id id = 0;

	for (ClassListIterator it = classes->begin(); it != classes->end(); )
	{
		if (!(*it)->isUsed())
		{
			mOut.addInfo("Removing class " + (*it)->getName()->getValue() + " because it is not used");
			
			classMap->erase((*it)->getName()->getValue());
			classes->erase(it++);
		}
		else
		{
			(*it)->setId(id++);
			++it;
		}		
	}

	classes = mProgram->getClasses();

	for (ClassListIterator it = classes->begin(); it != classes->end(); it++)
	{
		(*it)->setId(id++);
	}
}

