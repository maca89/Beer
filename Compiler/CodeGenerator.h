#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace CodeGeneration
	{
		class CodeGenerator
		{
		protected:

			//AST::ProgramNode * mProgram;
			//SemanticAnalysis::ProgramInfo * mProgram;
			
		public:

			INLINE CodeGenerator() { }

			void generatrClassFile(SemanticAnalysis::ProgramInfo * program, std::string name);


		};
	}
}