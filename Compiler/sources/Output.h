#pragma once

#include "prereq.h"

namespace Beer
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

		INLINE Output(int mode)
			:	mMode(mode),
				mErrorCount(0)
		{ }


		INLINE size_t getErrorCount() const
		{
			return mErrorCount;
		}


		void addInfo(const std::string & info);
		void addWarning(const std::string & warning);
		void addError(const std::string & error);
	};
}