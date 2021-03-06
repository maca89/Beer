#include "stdafx.h"
#include "MiliTimer.h"

using namespace Beer;


void MiliTimer::start()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&startTime);
}

float64 MiliTimer::stop()
{
	unsigned __int64 ticks = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	return (float64) (ticks - startTime) / frequency;
}