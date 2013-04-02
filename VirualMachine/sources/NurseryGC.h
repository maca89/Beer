#pragma once

#include "prereq.h"
#include "DynamicHeap.h"
#include "NativeThread.h"
#include "HeapThresholdNotify.h"

namespace Beer
{	
	class VirtualMachine;

	class NurseryGC : public NativeThread//, public HeapThresholdNotify
	{
	protected:

		typedef DynamicHeap NurseryHeap;

	protected:

		VirtualMachine* mVM;
		NurseryHeap* mAlloc;
		NurseryHeap* mCollect;
		NurseryHeap* mPromote;
		size_t mHeapSize;
		size_t mBlockSize;
		CRITICAL_SECTION mCS;

	public:

		NurseryGC(size_t initSize, size_t blockSize);
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

			if (!mem)
			{
				switchHeaps();
				::EnterCriticalSection(&mCS);
				mem = mAlloc->alloc(size);
				::LeaveCriticalSection(&mCS);
			}

			return mem;
		}

		void collect();

		// heap threshold notification

		void thresholdReached(Heap* heap, size_t threshold);
		
	protected:

		virtual void work()
		{

		}

		INLINE void switchHeaps()
		{
			NurseryHeap* temp = mAlloc;
			mAlloc = mPromote;
			mPromote = mCollect;
			mCollect = temp;
			//run();
		}
	};
}