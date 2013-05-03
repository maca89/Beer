#pragma once

#include "prereq.h"
#include "beer.tab.hpp"
#include "Output.h"
#include "SemanticAnalyser.h"
#include "CodeGenerator.h"
#include "ProgramNode.h"
#include "Program.h"

extern size_t lineNumber;
extern std::string filename;
extern Beer::Output * out;
extern Beer::AST::ClassDefinitionList * bClasses;
extern FILE * yyin;
void yyrestart(FILE *);
int yyparse();

namespace Beer
{
	typedef std::list<std::string> StringList;
	
	class BeerCompiler
	{
	protected:
		Output								mOut;
		StringList							mLibs;
		SemanticAnalysis::SemanticAnalyser	mSa;
		CodeGeneration::CodeGenerator		mCg;

		AST::ProgramNode					mProgram;
		SemanticAnalysis::Program			mPrg;

	public:

		BeerCompiler();
		
		void compile(StringList * filenames);
		AST::ClassDefinitionList * parseFile(const std::string & source);

	protected:

		void loadStdlibFiles();

	};
}