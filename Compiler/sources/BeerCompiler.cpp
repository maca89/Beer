#include "stdafx.h"

#include "BeerCompiler.h"

using namespace Beer;

BeerCompiler::BeerCompiler()
#ifdef _DEBUG
	: mOut(Output::Error | Output::Warning | Output::Info), mProgram(), mPrg(), mSa(mOut, &mPrg), mCg()
#else
	: mOut(Output::Error | Output::Warning), mProgram(), mPrg(), mSa(mOut, &mPrg), mCg()
#endif
{
}

void BeerCompiler::compile(StringList * filenames)
{
	loadStdlibFiles();

	AST::ClassDefinitionList * classes = NULL;

	for (StringList::iterator it = mLibs.begin(); it != mLibs.end(); it++)
	{
		classes = parseFile(*it);

		if (classes)
		{
			mProgram.addClasses(classes);

			delete classes;
		}
	}

	for (StringList::iterator it = filenames->begin(); it != filenames->end(); it++)
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

			mCg.generatrClassFile(&mPrg, classFileName);

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

AST::ClassDefinitionList * BeerCompiler::parseFile(const std::string & source)
{
	//FlexLexer * flex;

	lineNumber = 1;
	filename = source;
	out = &mOut;

	FILE * file;
	
	errno_t error = fopen_s(&file, source.c_str(), "r");

	if (!error) {

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

void BeerCompiler::loadStdlibFiles()
{
	WIN32_FIND_DATA file;
	bool working = true;
	std::wstring last;
	HANDLE myHandle = FindFirstFile(L"source/stdlib/*.beer", &file);

	if (myHandle != INVALID_HANDLE_VALUE)
	{
		last = file.cFileName;

		mLibs.push_back("source/stdlib/" + std::string(last.begin(), last.end()));
		
		while(working)
		{
			//CloseHandle(myHandle);

			FindNextFile(myHandle,&file);

			if(file.cFileName != last)
			{
				last = file.cFileName;
				mLibs.push_back("source/stdlib/" + std::string(last.begin(), last.end()));
			}
			else
			{
				working = false;
			}
		}
	}
}