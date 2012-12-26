#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class SemanticAnalyser
		{
		protected:

			Output & mOut;
			ProgramInfo * mProgram;
			ClassListBuilder mClb;
			ClassStructureBuilder mCsb;
			SemanticsChecker mSc;
			
		public:

			INLINE SemanticAnalyser(Output & out, ProgramInfo * program)
				: mOut(out), mProgram(program), mClb(out, program), mCsb(out, program), mSc(mOut, program) { }

			void analyse(AST::ProgramNode * program);
		};
	}
}