#pragma once
#include "prereq.h"

namespace Beer
{
	// may be entered multiple times by a signle thread without deadlock
	class CriticalSection
	{
	protected:
		CRITICAL_SECTION mCriticalSection;

	public:
		CriticalSection();
		~CriticalSection();

		void enter();
		bool tryEnter();
		void leave();

	protected:
		void create();
		void destroy();
	};

	// RAII principle
	class BlockingMutex
	{
	protected:
		CriticalSection* mMutex;

	public:
		INLINE BlockingMutex(CriticalSection* mutex) : mMutex(mutex)
		{
			mMutex->enter();
		}

		INLINE ~BlockingMutex()
		{
			mMutex->leave();
		}
	};

	class NonBlockingMutex
	{
	protected:
		CriticalSection* mMutex;
		bool mEntered;

	public:
		INLINE NonBlockingMutex(CriticalSection* mutex) : mMutex(mutex), mEntered(false)
		{
			mEntered = mMutex->tryEnter();
		}

		INLINE bool entered() const
		{
			return mEntered;
		}

		INLINE ~NonBlockingMutex()
		{
			if(mEntered)
			{
				mMutex->leave();
				mEntered = false;
			}
		}
	};
};