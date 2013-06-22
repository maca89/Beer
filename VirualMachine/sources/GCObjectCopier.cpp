#include "stdafx.h"
#include "GCObjectCopier.h"
#include "GenerationalGC.h"
#include "TaskScheduler.h"
#include "Class.h"
#include "RememeberdSet.h"

using namespace Beer;

void GCObjectCopier::traverseObjects(TaskScheduler* scheduler, RememberedSet* remSet)
{
	GCObject* nurseryGCObj = mTo->firstObject();
	//GCObject* matureGCObj = reinterpret_cast<GCObject*>(mMature->getMemory() + mMature->getFilled());

	TaskScheduler::TaskQueue* queue = scheduler->getActiveQueue();
	
	for (TaskScheduler::TaskQueue::iterator it = queue->begin(); it != queue->end(); it++)
	{
		copy(GCObject::get(*it));
	}

	for (RememberedSet::iterator it = remSet->begin(); it != remSet->end(); )
	{
		copy(it->first);

		for (RememberedSet::References::iterator rit = it->second->begin(); rit != it->second->end(); rit)
		{
			**rit = GCObject::get(**rit)->forward(); // forward all references to object
		}

		if (mMatureGC->contains(it->first->forward()))
		{
			remSet->erase(it++); // if was object moved, remove it from rememebered set
		}
		else
		{
			++it;
		}
	}


	while (nurseryGCObj->getSize() > 0/* || matureGCObj->getSize() > 0*/)
	{
		for (; mTo->isAllocObject(nurseryGCObj); nurseryGCObj = nurseryGCObj->nextObject())
		{
			Object* obj = nurseryGCObj->getObject();

			obj->getType()->getTraverser()(this, obj->getType(), obj);

			mForwarder.forward(nurseryGCObj);
		}

		/*for (; matureGCObj->getSize() > 0; matureGCObj = reinterpret_cast<GCObject*>(reinterpret_cast<byte*>(matureGCObj) + matureGCObj->getSize()))
		{
			Object* obj = matureGCObj->getObject();

			obj->getType()->getTraverser()(this, obj->getType(), obj);

			mForwarder.forward(matureGCObj);
		}*/
	}

#ifdef BEER_GC_DEBUGGING

	GCObject* obj = reinterpret_cast<GCObject*>(mTo->getMemory());

	for (; mTo->isAllocObject(obj); obj = obj->nextObject())
	{
		DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
		DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
		DBG_ASSERT(obj->getSize() == sizeof(GCObject) +
			obj->getObject()->getStaticSize() +
			(Object::OBJECT_CHILDREN_COUNT + obj->getObject()->getType()->getPropertiesCount()) * sizeof(Object*), BEER_WIDEN("object size is not right"));
	}

#endif
}

void GCObjectCopier::copy(GCObject* gcObj)
{
	GCObject* newGCObj = NULL;

	if (gcObj->getAge() < mPromotionAge)
	{
		newGCObj = reinterpret_cast<GCObject*>(mTo->alloc(gcObj->getSize()));
	}
	else
	{
#ifdef BEER_GC_STATS
		mGC->mStats.mPromoted++;
#endif
		newGCObj = reinterpret_cast<GCObject*>(mMatureGC->alloc(gcObj->getSize()));
	}

	::memcpy(newGCObj, gcObj, gcObj->getSize());

	gcObj->setPtr(newGCObj);

	if (gcObj->getAge() < mPromotionAge)
	{
		newGCObj->setAge(gcObj->getAge() + 1);
	}
}

void GCObjectCopier::traverseObjectPtr(Object** ptrToObject)
{
	Object* obj = *ptrToObject;

	if (isTraced(obj) && !GCObject::get(obj)->isForwarded())
	{
		copy(GCObject::get(obj));
	}
}