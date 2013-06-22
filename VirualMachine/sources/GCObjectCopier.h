#pragma once

#include "TraverseObjectReceiver.h"
#include "Object.h"
#include "ChildrenForwarder.h"
#include "FixedHeap.h"

namespace Beer
{
	class GCObject;
	class TaskScheduler;
	class GenerationalGC;
	class MatureGC;
	class RememberedSet;	

	class GCObjectCopier : protected TraverseObjectReceiver
	{
	protected:

		GenerationalGC* mGC;
		FixedHeap* mFrom;
		FixedHeap* mTo;
		MatureGC* mMatureGC;
		ChildrenForwarder mForwarder;

		uint32 mPromotionAge;

	public:

		INLINE GCObjectCopier(GenerationalGC* gc, FixedHeap* from, FixedHeap* to, MatureGC* matureGC, uint32 promotionAge = 3)
			:	mGC(gc),
				mFrom(from),
				mTo(to),
				mMatureGC(matureGC),
				mPromotionAge(promotionAge)
		{ }

		void traverseObjects(TaskScheduler* scheduler, RememberedSet* remSet);

	protected:

		void copy(GCObject* gcObj);

		INLINE bool isTraced(Object* obj)
		{
			return obj != NULL	&& // pointer is not NULL
				!Object::isInlineValue(obj) && // pointer is not inlined value
				mFrom->contains(reinterpret_cast<byte*>(obj)); // pointer is within collected heap
		}

		virtual void traverseObjectPtr(Object** ptrToObject);
	};
};