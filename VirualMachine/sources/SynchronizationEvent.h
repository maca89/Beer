#pragma once
#include "prereq.h"

namespace Beer
{
	class SynchronizationEvent
	{
	public:
		enum Mode
		{
			EVENT_AUTO_RESET = 0,
			EVENT_MANUAL_RESET = 1
		};

	protected:
		HANDLE mHandle;

	public:
		SynchronizationEvent(Mode mode = EVENT_AUTO_RESET);
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