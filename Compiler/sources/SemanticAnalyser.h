#pragma once

#include "prereq.h"
#include "ClassListBuilder.h"
#include "ClassStructureBuilder.h"
#include "SemanticsChecker.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class SemanticAnalyser
		{
		protected:

			Output & mOut;
			Program * mProgram;
			ClassListBuilder mClb;
			ClassStructureBuilder mCsb;
			SemanticsChecker mSc;
			
		public:

			INLINE SemanticAnalyser(Output & out, Program * program)
				:	mOut(out),
					mProgram(program),
					mClb(out, program),
					mCsb(out, program),
					mSc(mOut, program)
			{ }

			void analyse(AST::ProgramNode * program);
		};
	}
}