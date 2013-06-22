#pragma once

#include "prereq.h"
#include "Mutex.h"
#include "FixedHeap.h"

namespace Beer
{	
	class AllocationBlock;
	class TaskScheduler;
	class GenerationalGC;
	class RememberedSet;

	class NurseryGC
	{
	protected:

		typedef std::list<AllocationBlock*> BlockList;

	protected:

		GenerationalGC* mGC;

		byte* mMemStart; 
		byte* mMemEnd;

		FixedHeap* mFrom;
		FixedHeap* mTo;

		uint32 mBlockSize;

		BlockList mBlocks;

		CriticalSection mMutex;

	public:
		
		NurseryGC(GenerationalGC* gc, uint32 size, uint32 blockSize);
		~NurseryGC();

		INLINE GenerationalGC* getGenerationalGC()
		{
			return mGC;
		}

		byte* alloc(uint32 size);
		byte* allocHeap(uint32 size);
		Heap* createHeap();

		void collect(TaskScheduler* scheduler, RememberedSet* remSet);

		void afterCollection();

		INLINE bool contains(byte* ptr)
		{
			return ptr >= mMemStart && ptr < mMemEnd;
		}
	};
};