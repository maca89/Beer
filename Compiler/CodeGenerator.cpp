#include "stdafx.h"

void BeerlangCompiler::CodeGeneration::CodeGenerator::generatrClassFile(SemanticAnalysis::ProgramInfo * program, std::string name)
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