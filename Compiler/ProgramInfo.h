#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		typedef int16 length;

		class ProgramInfo
		{
		protected:

			DescriptorDictionary mDictionary;
			ClassList mExternalClasses;
			ClassList mClasses;
			ClassMap mClassMap;

		public:

			ProgramInfo();

			INLINE DescriptorDictionary * getDescriptorDictionary() { return &mDictionary; }

			INLINE ClassList * getExternalClasses() { return &mExternalClasses; }
			INLINE ClassList * getClasses() { return &mClasses; }
			INLINE ClassMap * getClassMap() { return &mClassMap; }

			ClassInfo * getClass(std::string name);

			//ClassInfo * addExternalClass(std::string name, class_flags flags, Output & out);
			ClassInfo * addClass(std::string name, class_flags flags, Output & out);
		};
	}
}