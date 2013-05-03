#pragma once

#include "prereq.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class Program;
	}

	namespace CodeGeneration
	{
		class CodeGenerator
		{
		public:

			INLINE CodeGenerator() { }

			void generatrClassFile(SemanticAnalysis::Program * program, const std::string & name);

		};
	}
}