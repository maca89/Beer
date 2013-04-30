#pragma once
#include "prereq.h"

namespace Beer
{
	class SynchronizationEvent
	{
	protected:
		HANDLE mHandle;

	public:
		SynchronizationEvent();
		~SynchronizationEvent();

		void reset();
		void fire();
		bool wait(uint32 maxTime = INFINITE);

		HANDLE getHandle() const;
		HANDLE& getHandle();
	};

	// inline definitions

	INLINE HANDLE SynchronizationEvent::getHandle() const
	{
		return mHandle;
	}
	
	INLINE HANDLE& SynchronizationEvent::getHandle()
	{
		return mHandle;
	}
};