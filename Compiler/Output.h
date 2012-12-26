#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	class Output
	{
	public:

		enum Mode
		{
			Error = 1,
			Warning = 2,
			Info = 4
		};
		
	protected:

		int mMode;
		size_t mErrorCount;

	public:

		INLINE Output(int mode) : mMode(mode), mErrorCount(0) { }

		INLINE size_t getErrorCount() const { return mErrorCount; }

		void addInfo(std::string info);
		void addWarning(std::string warning);
		void addError(std::string error);
	};
}