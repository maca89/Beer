#pragma once

#include "prereq.h"
#include "Mutex.h"
#include "FixedHeap.h"
#include "TraverseObjectReceiver.h"
#include "ChildrenForwarder.h"

namespace Beer
{	
	class AllocationBlock;
	class TaskScheduler;
	class GenerationalGC;
	class RememberedSet;

	class NurseryGC : protected TraverseObjectReceiver
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
		RememberedSet* mRemSet;

		ChildrenForwarder mForwarder;

		CriticalSection mMutex;

	public:

		static const uint32 PromotionAge;

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
		void invalidateBlocks();

		INLINE FixedHeap* getFromHeap()
		{
			return mFrom;
		}

		INLINE FixedHeap* getToHeap()
		{
			return mTo;
		}

		INLINE bool contains(void* ptr)
		{
			return ptr >= mMemStart && ptr < mMemEnd;
		}

		INLINE void switchHeaps()
		{
			FixedHeap* temp = mFrom;
			mFrom = mTo;
			mTo = temp;
		}

	protected:
		
		void copy(GCObject* gcObj);
		virtual void traverseObjectPtr(Object** ptrToObject);

		INLINE bool isTraced(Object* obj)
		{
			return obj != NULL	&& // pointer is not NULL
				!Object::isInlineValue(obj) && // pointer is not inlined value
				mFrom->contains(obj); // pointer is within collected heap
		}

	};
};