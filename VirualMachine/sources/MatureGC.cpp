#include "stdafx.h"
#include "MatureGC.h"
#include "FixedHeap.h"
#include "GenerationalGC.h"
#include "TaskScheduler.h"
#include "Object.h"
#include "Class.h"

using namespace Beer;

MatureGC::MatureGC(GenerationalGC* gc, uint32 size, NurseryGC* nurseryGC)
	:	mRemSet(NULL),
		mNeedCollect(false)
{
	mGC = gc;
	mNurseryGC = nurseryGC;

	mMemStart = new byte[size * 2];
	mMemEnd = mMemStart + size * 2;

	mFrom = new FixedHeap(mMemStart, size);
	mFrom->init();

	mTo = new FixedHeap(mMemStart + size, size);
	mTo->init();
}

MatureGC::~MatureGC()
{
	delete mMemStart;
	delete mFrom;
	delete mTo;
}

byte* MatureGC::alloc(uint32 size)
{
	BlockingMutex mutex(&mMutex);

	byte* obj = mFrom->alloc(size);

#ifdef BEER_GC_STATS
	if (obj) mGC->mStats.mAllocatedMature += size;
#endif

	if ((float)mFrom->getFilled() / mFrom->getSize() > 0.75f)
	{
		mNeedCollect = true;
	}

	return obj;
}

void MatureGC::collect(TaskScheduler* scheduler, RememberedSet* remSet)
{
	mRemSet = remSet;

	FixedHeap* nurseryToHeap = mNurseryGC->getToHeap();

	GCObject* nurseryGCObj = nurseryToHeap->firstObject();
	GCObject* matureGCObj = mTo->firstObject();

	TaskScheduler::TaskQueue* queue = scheduler->getActiveQueue();

	for (TaskScheduler::TaskQueue::iterator it = queue->begin(); it != queue->end(); it++)
	{
		if (mNurseryGC->contains(*it))
		{
			copyNursery(GCObject::get(*it));
		}
		else if (contains(*it))
		{
			copyMature(GCObject::get(*it));
		}
		else
		{
			DBG_ASSERT(false, BEER_WIDEN("object is not from nursery nor mature heap"));
		}
	}

	while (nurseryToHeap->allocated(nurseryGCObj) || mTo->allocated(matureGCObj))
	{
		for (; nurseryToHeap->allocated(nurseryGCObj); nurseryGCObj = nurseryGCObj->nextObject())
		{
			Object* obj = nurseryGCObj->getObject();
			Class* type = obj->getType();

			type->getTraverser()(this,type, obj);

			mForwarder.forward(nurseryGCObj);
		}

		for (; mTo->allocated(matureGCObj); matureGCObj = matureGCObj->nextObject())
		{
			Object* obj = matureGCObj->getObject();
			Class* type = obj->getType();

			type->getTraverser()(this, type, obj);

			mForwarder.forward(matureGCObj);
		}
	}
#ifdef BEER_GC_DEBUGGING

	{
		GCObject* obj = mTo->firstObject();

		for (; mTo->allocated(obj); obj = obj->nextObject())
		{
			DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
			DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
			DBG_ASSERT(obj->getSize() == sizeof(GCObject) +
				obj->getObject()->getStaticSize() +
				(Object::OBJECT_CHILDREN_COUNT + obj->getObject()->getType()->getPropertiesCount()) * sizeof(Object*), BEER_WIDEN("object size is not right"));
		}

	}
	{
		GCObject* obj = nurseryToHeap->firstObject();

		for (; nurseryToHeap->allocated(obj); obj = obj->nextObject())
		{
			DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
			DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
			DBG_ASSERT(obj->getSize() == sizeof(GCObject) +
				obj->getObject()->getStaticSize() +
				(Object::OBJECT_CHILDREN_COUNT + obj->getObject()->getType()->getPropertiesCount()) * sizeof(Object*), BEER_WIDEN("object size is not right"));
		}
	}
#endif

	mNeedCollect = false;
	mNurseryGC->invalidateBlocks();
	switchHeaps();
	mNurseryGC->switchHeaps();
	mGC->stopCollection(GenerationalGC::GEN_MATURE);
}

#ifdef BEER_GC_DEBUGGING

class MatureGCDebugTraverser : public TraverseObjectReceiver
{
protected:

	Object* mObj;

public:

	MatureGCDebugTraverser(Object* obj)
		:	mObj(obj)
	{
	}

	virtual void traverseObjectPtr(Object** ptrToObject)
	{
		if (!Object::isInlineValue(*ptrToObject) && *ptrToObject == mObj)
		{
			DBG_ASSERT(false, BEER_WIDEN("a pointer was left out"));
		}
	}
};

#endif

void MatureGC::afterCollection()
{
#ifdef BEER_GC_DEBUGGING
	{
		GCObject* obj = mFrom->firstObject();

		for (; mFrom->allocated(obj); obj = obj->nextObject())
		{
			DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
			DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
		}
	}

	{
		Object** obj = reinterpret_cast<Object**>(mFrom->getMemory());

		while (mFrom->contains(obj))
		{
			if (mTo->allocated(*obj) || mNurseryGC->getToHeap()->allocated(*obj))
			{
				GCObject* parent = mFrom->firstObject();

				while (reinterpret_cast<Object**>(parent->nextObject()) < obj)
				{
					parent = parent->nextObject();
				}

				MatureGCDebugTraverser traverser(*obj);

				parent->getObject()->getType()->getTraverser()(&traverser, parent->getObject()->getType(), parent->getObject());

				DBG_ASSERT(false, BEER_WIDEN("pointers from from-space into to-space"));
			}

			obj++;
		}
	}

	{
		GCObject* obj = mNurseryGC->getFromHeap()->firstObject();

		for (; mNurseryGC->getFromHeap()->allocated(obj); obj = obj->nextObject())
		{
			DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
			DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
		}
	}

	{
		Object** obj = reinterpret_cast<Object**>(mNurseryGC->getFromHeap()->getMemory());

		while (mNurseryGC->getFromHeap()->contains(obj))
		{
			if (mNurseryGC->getToHeap()->allocated(*obj) || mTo->allocated(*obj))
			{
				GCObject* parent = mNurseryGC->getFromHeap()->firstObject();

				while (reinterpret_cast<Object**>(parent->nextObject()) < obj)
				{
					parent = parent->nextObject();
				}

				MatureGCDebugTraverser traverser(*obj);

				parent->getObject()->getType()->getTraverser()(&traverser, parent->getObject()->getType(), parent->getObject());

				DBG_ASSERT(false, BEER_WIDEN("pointers from from-space into to-space"));
			}

			obj++;
		}
	}
#endif

	mFrom->clear();
	mNurseryGC->getFromHeap()->clear();
}

void MatureGC::copyNursery(GCObject* gcObj)
{
	GCObject* newGCObj = NULL;

	if (gcObj->getAge() < NurseryGC::PromotionAge)
	{
		newGCObj = reinterpret_cast<GCObject*>(mNurseryGC->getToHeap()->alloc(gcObj->getSize()));
	}
	else
	{
#ifdef BEER_GC_STATS
		mGC->mStats.mPromoted++;
#endif
		newGCObj = reinterpret_cast<GCObject*>(mTo->alloc(gcObj->getSize()));

		mRemSet->remove(gcObj);
	}

	::memcpy(newGCObj, gcObj, gcObj->getSize());

	gcObj->setPtr(newGCObj);

	if (gcObj->getAge() < NurseryGC::PromotionAge)
	{
		newGCObj->setAge(gcObj->getAge() + 1);
	}
}

void MatureGC::copyMature(GCObject* gcObj)
{
	GCObject* newGCObj = reinterpret_cast<GCObject*>(mTo->alloc(gcObj->getSize()));

	::memcpy(newGCObj, gcObj, gcObj->getSize());

	gcObj->setPtr(newGCObj);
}

void MatureGC::traverseObjectPtr(Object** ptrToObject)
{
	Object* obj = *ptrToObject;

	if (isTraced(obj) && !GCObject::get(obj)->isForwarded())
	{
		if (mNurseryGC->getFromHeap()->contains(obj))
		{
			copyNursery(GCObject::get(obj));
		}
		else if (mFrom->contains(obj))
		{
			copyMature(GCObject::get(obj));
		}
	}
}