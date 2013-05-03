#include "stdafx.h"

#include "CodeGenerator.h"

#include "ClassFileGenerator.h"

using namespace Beer::CodeGeneration;

void CodeGenerator::generatrClassFile(::Beer::SemanticAnalysis::Program * program, const std::string & name)
{
	std::ofstream file;

	file.open(name, std::ios::binary | std::ios::out);

	if (file.is_open())
	{
		ClassFileGenerator cfg(file);

		cfg.generateClassFile(program);
	}
	
	file.close();
}