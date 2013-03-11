#pragma once

#include "prereq.h"
#include "AlignedHeap.h"
#include "NativeThread.h"

namespace Beer
{
	class NurseryGC : public NativeThread
	{
	protected:

		AlignedHeap* mAlloc;
		AlignedHeap* mCollect;
		AlignedHeap* mPromote;
		size_t mHeapSize;
		size_t mBlockSize;
		CRITICAL_SECTION mCS;

	public:

		NurseryGC(uint8 nurseryBitSize, size_t blockSize);
		~NurseryGC();

		void init();

		INLINE AlignedHeap* getAllocHeap()
		{
			return mAlloc;
		}

		Heap* createHeap();
		

		INLINE void* alloc(uint32 size)
		{
			::EnterCriticalSection(&mCS);
			void* mem = mAlloc->alloc(size);
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

	protected:

		virtual void work()
		{

		}

		INLINE void switchHeaps()
		{
			AlignedHeap* temp = mAlloc;
			mAlloc = mPromote;
			mPromote = mCollect;
			mCollect = temp;
			run();
		}
	};
}