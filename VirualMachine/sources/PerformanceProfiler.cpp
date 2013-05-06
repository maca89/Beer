#include "stdafx.h"
#include "PerformanceProfiler.h"
#include "Interlocked.h"

using namespace Beer;


PerformanceProfiler PerformanceProfiler::gInstance;

PerformanceProfiler::PerformanceProfiler()
{
	mMonoCachesWait = 0;
	mPolyCachesWait = 0;
}

PerformanceProfiler::~PerformanceProfiler()
{
}

void add(float32* dest, float32 add)
{
	while(true)
	{
		float32 oldVal = *dest;
		float32 newVal = oldVal + add;

		if(Interlocked::CompareAndSwap32(reinterpret_cast<int32*>(dest), *reinterpret_cast<int32*>(&oldVal), *reinterpret_cast<int32*>(&newVal)))
		{
			break;
		}
	}
}

void PerformanceProfiler::addMonoCacheWait(float32 time)
{
	add(&mMonoCachesWait, time);
}

void PerformanceProfiler::addPolyCacheWait(float32 time)
{
	add(&mPolyCachesWait, time);
}
