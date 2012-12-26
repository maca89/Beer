#include "stdafx.h"

#include <string>
#include "beerlang.tab.hpp"
#include <Windows.h>

BeerlangCompiler::BeerlangCompiler::BeerlangCompiler()
#ifdef _DEBUG
	: mOut(Output::Error | Output::Warning | Output::Info), mProgram(), mProgramInfo(), mSa(mOut, &mProgramInfo), mCg()
#else
	: mOut(Output::Error | Output::Warning), mProgram(), mProgramInfo(), mSa(mOut, &mProgramInfo), mCg()
#endif
{
}

void BeerlangCompiler::BeerlangCompiler::compile(StringList * filenames)
{
	loadStdlibFiles();

	AST::ClassDefinitionList * classes = NULL;

	for (StringListIterator it = mLibs.begin(); it != mLibs.end(); it++)
	{
		classes = parseFile(*it);

		if (classes)
		{
			mProgram.addClasses(classes);

			delete classes;
		}
	}

	for (StringListIterator it = filenames->begin(); it != filenames->end(); it++)
	{
		classes = parseFile(*it);

		if (classes)
		{
			mProgram.addClasses(classes);

			delete classes;
		}
	}

	if (mOut.getErrorCount() == 0)
	{
		mSa.analyse(&mProgram);

		if (mOut.getErrorCount() == 0)
		{
			std::string filename = filenames->front();
			std::string classFileName;

			size_t dotPos = filename.find_last_of('.');

			if (dotPos != std::string::npos)
			{
				classFileName = filename.substr(0, dotPos) + ".class";
			}
			else
			{
				classFileName = filename + ".class";
			}

			mCg.generatrClassFile(&mProgramInfo, classFileName);

			std::cout << std::endl << "Everything OK, class file: " << classFileName << std::endl;
		}
		else
		{
			std::cout << std::endl << "There are some errors in semantic analysis - aborting compilation" << std::endl;
		}
	}
	else
	{
		std::cout << std::endl << "There are some errors in lexical analysis - aborting compilation" << std::endl;
	}
}

BeerlangCompiler::AST::ClassDefinitionList * BeerlangCompiler::BeerlangCompiler::parseFile(const std::string & source)
{
	lineNumber = 1;
	filename = source;
	out = &mOut;

	FILE * file = fopen(source.c_str(), "r");

	if (file) {

		yyin = file;

		yyrestart(file);

		do {
			yyparse();
		}
		while (!feof(yyin));

		fclose(file);


	}

	AST::ClassDefinitionList * temp = bClasses;

	bClasses = NULL;

	return temp;
}

void BeerlangCompiler::BeerlangCompiler::loadStdlibFiles()
{
	WIN32_FIND_DATA file;
	bool working = true;
	std::string last;
	HANDLE myHandle = FindFirstFile("stdlib/*.beer",&file);

	if (myHandle != INVALID_HANDLE_VALUE)
	{
		last = file.cFileName;
		mLibs.push_back("stdlib/" + last);
		
		while(working)
		{
			FindNextFile(myHandle,&file);

			if(file.cFileName != last)
			{
				last = file.cFileName;
				mLibs.push_back("stdlib/" + last);
			}
			else
			{
				working = false;
			}
		}
	}

}