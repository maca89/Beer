#include "stdafx.h"

#include "Output.h"

using namespace Beer;

void Output::addInfo(const std::string & info)
{
	if (mMode & Info)
	{
		std::cout << "INFO: " << info << std::endl;
	}
}

void Output::addWarning(const std::string & warning)
{
	if (mMode & Warning)
	{
		std::cout << "WARNING: " << warning << std::endl;
	}
}

void Output::addError(const std::string & error)
{
	if (mMode & Error)
	{
		std::cout << "ERROR: " << error << std::endl;
	}

	mErrorCount++;
}