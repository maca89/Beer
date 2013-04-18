#pragma once

#include "prereq.h"
#include "NativeThread.h"
#include "NotifyHeap.h"
#include <queue>
#include "TaskScheduler.h"

namespace Beer
{	
	class VirtualMachine;
	class GenerationalGC;

	class NurseryGC : public NativeThread, public HeapThresholdNotify
	{
	protected:

		typedef NotifyHeap NurseryHeap;

	protected:

		VirtualMachine* mVM;
		GenerationalGC* mGC;
		NurseryHeap* mAlloc;
		NurseryHeap* mCollect;
		NurseryHeap* mPromote;
		size_t mHeapSize;
		size_t mBlockSize;
		size_t mCollectionCount;
		CRITICAL_SECTION mCS;
		HANDLE mThreadsSuspendedEvent;

	public:

		NurseryGC(GenerationalGC* gc, size_t initSize, size_t blockSize);
		~NurseryGC();

		void init(VirtualMachine* vm);

		INLINE Heap* getAllocHeap()
		{
			return mAlloc;
		}

		Heap* createHeap();		

		INLINE byte* alloc(uint32 size)
		{
			::EnterCriticalSection(&mCS);
			byte* mem = mAlloc->alloc(size);
			::LeaveCriticalSection(&mCS);

			return mem;
		}

		// not used
		void collect();

		// heap threshold notification
		void thresholdReached(Heap* heap, size_t threshold);

		void threadsSuspended();
			
	protected:

		virtual void work();

		INLINE void switchHeaps()
		{
			NurseryHeap* temp = mAlloc;
			mAlloc = mPromote;
			mPromote = mCollect;
			mCollect = temp;

			mAlloc->clear();
		}
	};
}