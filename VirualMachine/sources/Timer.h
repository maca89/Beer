#pragma once
#include "prereq.h"

namespace Beer
{
	class MiliTimer // measures in miliseconds
	{
	protected:
		unsigned __int64 frequency;
		unsigned __int64 startTime;

	public:	
		INLINE MiliTimer()  {}
		INLINE ~MiliTimer() {}

		void start();
		double stop();

	protected:
	};
};
