#pragma once
#include "prereq.h"


namespace Beer
{
	class PerformanceProfiler
	{
	protected:
		float32 mMonoCachesWait;
		float32 mPolyCachesWait;

		static PerformanceProfiler gInstance;

	public:
		PerformanceProfiler();
		~PerformanceProfiler();

		void addMonoCacheWait(float32 time);
		void addPolyCacheWait(float32 time);

		float getMonoCachesWaiting();
		float getPolyCachesWaiting();

		static PerformanceProfiler* getInstance();
	};


	// inline

	INLINE PerformanceProfiler* PerformanceProfiler::getInstance()
	{
		return &gInstance;
	}

	INLINE float PerformanceProfiler::getMonoCachesWaiting()
	{
		return mMonoCachesWait;
	}

	INLINE float PerformanceProfiler::getPolyCachesWaiting()
	{
		return mPolyCachesWait;
	}
};