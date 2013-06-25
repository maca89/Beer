#include "stdafx.h"
#include "NurseryGC.h"
#include "FixedHeap.h"
#include "AllocationBlock.h"
#include "GenerationalGC.h"
#include "TaskScheduler.h"
#include "GCObjectCopier.h"
#include "GCObject.h"
#include "Object.h"
#include "Class.h"
#include "TraverseObjectReceiver.h"

using namespace Beer;

const uint32 NurseryGC::PromotionAge = 3;

NurseryGC::NurseryGC(GenerationalGC* gc, uint32 size, uint32 blockSize)
{
	mGC = gc;

	mMemStart = new byte[size * 2];
	mMemEnd = mMemStart + size * 2;

	mFrom = new FixedHeap(mMemStart, size);
	mFrom->init();

	mTo = new FixedHeap(mMemStart + size, size);
	mTo->init();

	mBlockSize = blockSize;
}

NurseryGC::~NurseryGC()
{
	delete mMemStart;
	delete mFrom;
	delete mTo;
}

byte* NurseryGC::alloc(uint32 size)
{
	BlockingMutex mutex(&mMutex);

	byte* obj = mFrom->alloc(size);

	if (!obj)
	{
		mGC->startCollection(GenerationalGC::GEN_NURSERY); // temporary to start mature collection

		obj = mTo->alloc(size);

		if (!obj)
		{
			// this is a problem - try allocate from mature heap
			throw NotEnoughMemoryException(BEER_WIDEN("Not enough memory"));
		}
	}

	return obj;
}

byte* NurseryGC::allocHeap(uint32 size)
{
	BlockingMutex mutex(&mMutex);

	byte* obj = mFrom->alloc(size);

#ifdef BEER_GC_STATS
	if (obj) mGC->mStats.mAllocatedNursery += size;
#endif

	if (!obj) mGC->startCollection(GenerationalGC::GEN_NURSERY); // temporary to start mature collection

	return obj;
}

Heap* NurseryGC::createHeap()
{
	BlockingMutex mutex(&mMutex);

	AllocationBlock* block = new AllocationBlock(this, mBlockSize, static_cast<uint32>(mBlockSize));
	mBlocks.push_back(block);
	return block;
}

void NurseryGC::collect(TaskScheduler* scheduler, RememberedSet* remSet)
{
	mRemSet = remSet;

	MatureGC* matureGC = mGC->getMatureGC();
	GCObject* nurseryGCObj = mTo->firstObject();
	GCObject* matureGCObj = matureGC->firstObject();

	TaskScheduler::TaskQueue* queue = scheduler->getActiveQueue();

	for (TaskScheduler::TaskQueue::iterator it = queue->begin(); it != queue->end(); it++)
	{
		copy(GCObject::get(*it));
	}

	for (RememberedSet::iterator it = remSet->begin(); it != remSet->end(); )
	{
		copy(it->first);

		if (matureGC->contains(it->first->forward()))
		{
			remSet->erase(it++); // if was object moved, remove it from rememebered set
		}
		else
		{
			++it;
		}
	}

	while (mTo->allocated(nurseryGCObj) || matureGC->allocated(matureGCObj))
	{
		for (; mTo->allocated(nurseryGCObj); nurseryGCObj = nurseryGCObj->nextObject())
		{
			Object* obj = nurseryGCObj->getObject();
			Class* type = obj->getType();

			type->getTraverser()(this, type, obj);

			mForwarder.forward(nurseryGCObj);
		}

		for (; matureGC->allocated(matureGCObj); matureGCObj = matureGCObj->nextObject())
		{
			Object* obj = matureGCObj->getObject();
			Class* type = obj->getType();

			type->getTraverser()(this, type, obj);

			mForwarder.forward(matureGCObj);
		}
	}

	switchHeaps();
	mGC->stopCollection(GenerationalGC::GEN_NURSERY);
}

void NurseryGC::copy(GCObject* gcObj)
{
	GCObject* newGCObj = NULL;

	if (gcObj->getAge() < NurseryGC::PromotionAge)
	{
		newGCObj = reinterpret_cast<GCObject*>(mTo->alloc(gcObj->getSize()));
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

void NurseryGC::traverseObjectPtr(Object** ptrToObject)
{
	Object* obj = *ptrToObject;

	if (isTraced(obj) && !GCObject::get(obj)->isForwarded())
	{
		copy(GCObject::get(obj));
	}
}


#ifdef BEER_GC_DEBUGGING

class NurseryGCDebugTraverser : public TraverseObjectReceiver
{
protected:

	Object* mObj;

public:

	NurseryGCDebugTraverser(Object* obj)
		:	mObj(obj)
	{
	}

	virtual void traverseObjectPtr(Object** ptrToObject)
	{
		if (*ptrToObject == mObj)
		{
			DBG_ASSERT(false, BEER_WIDEN("a pointer was left out"));
		}
	}
};

#endif

void NurseryGC::afterCollection()
{
#ifdef BEER_GC_DEBUGGING
	{
		GCObject* obj = reinterpret_cast<GCObject*>(mFrom->getMemory());

		for (; mFrom->allocated(obj); obj = obj->nextObject())
		{
			DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
			DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
		}
	}

	{
		Object** obj = reinterpret_cast<Object**>(mFrom->getMemory());

		while (mFrom->allocated(obj))
		{
			if (mTo->allocated(*obj))
			{
				GCObject* parent = mFrom->firstObject();

				while (reinterpret_cast<Object**>(parent->nextObject()) < obj)
				{
					parent = parent->nextObject();
				}

				NurseryGCDebugTraverser traverser(*obj);

				parent->getObject()->getType()->getTraverser()(&traverser, parent->getObject()->getType(), parent->getObject());

				DBG_ASSERT(false, BEER_WIDEN("pointers from from-space into to-space"));
			}
			
			obj++;
		}
	}

#endif

	mTo->clear();
}

void NurseryGC::invalidateBlocks()
{
	for (BlockList::iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
	{
		(*it)->invalidate();
	}
}

