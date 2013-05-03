#include "stdafx.h"

#include "BeerCompiler.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "");

	if (argc > 1)
	{
		Beer::BeerCompiler bc;

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

	//system("PAUSE");
}

