#pragma once

#include "prereq.h"
#include "Mutex.h"
#include "FixedHeap.h"
#include "TraverseObjectReceiver.h"
#include "ChildrenForwarder.h"

namespace Beer
{
	class GCObject;
	class GenerationalGC;
	class TaskScheduler;
	class RememberedSet;
	class NurseryGC;

	class MatureGC : protected TraverseObjectReceiver
	{
	protected:

		GenerationalGC* mGC;

		RememberedSet* mRemSet;

		byte* mMemStart; 
		byte* mMemEnd;

		FixedHeap* mFrom;
		FixedHeap* mTo;

		bool mNeedCollect;

		ChildrenForwarder mForwarder;

		NurseryGC* mNurseryGC;

		CriticalSection mMutex;

	public:

		MatureGC(GenerationalGC* gc, uint32 size, NurseryGC* nurseryGC);
		~MatureGC();

		byte* alloc(uint32 size);

		void collect(TaskScheduler* scheduler, RememberedSet* remSet);

		void afterCollection();

		INLINE bool needCollect() const
		{
			return mNeedCollect;
		}

		INLINE void needCollect(bool value)
		{
			mNeedCollect = value;
		}

		INLINE GCObject* firstObject()
		{
			return mFrom->firstObject();
		}

		INLINE bool contains(void* obj)
		{
			return reinterpret_cast<byte*>(obj) >= mMemStart && reinterpret_cast<byte*>(obj) < mMemEnd;
		}

		INLINE bool allocated(void* obj)
		{
			return mFrom->allocated(obj) || mTo->allocated(obj);
		}

		INLINE void switchHeaps()
		{
			FixedHeap* temp = mFrom;
			mFrom = mTo;
			mTo = temp;
		}

	protected:

		void copyNursery(GCObject* gcObj);
		void copyMature(GCObject* gcObj);
		virtual void traverseObjectPtr(Object** ptrToObject);

		INLINE bool isTraced(Object* obj)
		{
			return obj != NULL	&& // pointer is not NULL
				!Object::isInlineValue(obj);// pointer is not inlined value				
		}
	};
};