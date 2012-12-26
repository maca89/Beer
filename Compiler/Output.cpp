#include "stdafx.h"

void BeerlangCompiler::Output::addInfo(std::string info)
{
	if (mMode & Info)
	{
		std::cout << "INFO: " << info << std::endl;
	}
}

void BeerlangCompiler::Output::addWarning(std::string warning)
{
	if (mMode & Warning)
	{
		std::cout << "WARNING: " << warning << std::endl;
	}
}

void BeerlangCompiler::Output::addError(std::string error)
{
	if (mMode & Error)
	{
		std::cout << "ERROR: " << error << std::endl;
	}

	mErrorCount++;
}