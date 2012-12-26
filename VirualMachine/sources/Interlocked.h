#pragma once
#include "prereq.h"


namespace Beer
{
	class Interlocked
	{
	public:

		// 32bit
		static bool CompareAndSwap32(int32* dest, int32 oldValue, int32 newValue);
	};
};