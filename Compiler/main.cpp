#include "stdafx.h"

int main(int argc, char ** argv)
{
	if (argc > 1)
	{
		BeerlangCompiler::BeerlangCompiler bc;

		std::list<std::string> filenames;

		for (int i = 1; i < argc; i++)
		{
			filenames.push_back(argv[i]);
		}

		bc.compile(&filenames);
	}
	else
	{
		std::cout << "Error source file name missing";
	}
}

