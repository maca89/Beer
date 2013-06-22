#pragma once

#include "prereq.h"
#include "Mutex.h"
#include "FixedHeap.h"

namespace Beer
{
	class GCObject;
	class GenerationalGC;
	class TaskScheduler;
	class RememberedSet;

	class MatureGC
	{
	protected:

		GenerationalGC* mGC;

		byte* mMemStart; 
		byte* mMemEnd;

		FixedHeap* mFrom;
		FixedHeap* mTo;

		CriticalSection mMutex;

	public:
		
		MatureGC(GenerationalGC* gc, uint32 size);
		~MatureGC();

		byte* alloc(uint32 size);

		void collect(TaskScheduler* scheduler, RememberedSet* remSet);

		void afterCollection();

		INLINE GCObject* firstObject()
		{
			return mFrom->firstObject();
		}

		INLINE bool contains(void* obj)
		{
			return reinterpret_cast<byte*>(obj) >= mMemStart && reinterpret_cast<byte*>(obj) < mMemEnd;
		}
	};
};