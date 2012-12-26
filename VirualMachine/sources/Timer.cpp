#include "stdafx.h"
#include "Timer.h"

using namespace Beer;


void MiliTimer::start()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&startTime);
}

double MiliTimer::stop()
{
	unsigned __int64 ticks = 0;
	QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
	return (double) (ticks - startTime) / frequency;
}