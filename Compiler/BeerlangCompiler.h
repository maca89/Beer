#pragma once

#include "stdafx.h"

extern size_t lineNumber;
extern std::string filename;
extern BeerlangCompiler::Output * out;
extern BeerlangCompiler::AST::ClassDefinitionList * bClasses;
extern FILE * yyin;
void yyrestart(FILE *);
int yyparse();

namespace BeerlangCompiler
{
	typedef std::list<std::string> StringList;
	typedef std::list<std::string>::iterator StringListIterator;

	class BeerlangCompiler
	{
	protected:
		Output mOut;
		StringList mLibs;
		SemanticAnalysis::SemanticAnalyser mSa;
		CodeGeneration::CodeGenerator mCg;

		AST::ProgramNode mProgram;
		SemanticAnalysis::ProgramInfo mProgramInfo;

	public:

		BeerlangCompiler();
		
		void compile(StringList * filenames);
		AST::ClassDefinitionList * parseFile(const std::string & source);

	protected:

		void loadStdlibFiles();

	};
}

