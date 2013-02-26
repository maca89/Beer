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
		INLINE MiliTimer()  { /*start();*/ }
		INLINE ~MiliTimer() {}

		void start();
		float64 stop();

	protected:
	};
};
