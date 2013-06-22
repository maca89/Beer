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
		mGC->nurseryFull();

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

	if (!obj) mGC->nurseryFull();

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
	GCObjectCopier copier(mGC, mFrom, mTo, mGC->getMatureGC());
	copier.traverseObjects(scheduler, remSet);

	for (BlockList::iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
	{
		(*it)->invalidate();
	}

	FixedHeap* temp = mFrom;
	mFrom = mTo;
	mTo = temp;

	mGC->restartThreads();
}


#ifdef BEER_GC_DEBUGGING

class Trav : public TraverseObjectReceiver
{
protected:

	Object* mObj;

public:

	Trav(Object* obj)
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

		for (; mFrom->isAllocObject(obj); obj = obj->nextObject())
		{
			DBG_ASSERT(obj->getPtr() == NULL, BEER_WIDEN("obj should not have forwarding pointer"));
			DBG_ASSERT(obj->getObject()->getType() != NULL, BEER_WIDEN("obj should have type"));
			DBG_ASSERT(obj->getSize() == sizeof(GCObject) +
				obj->getObject()->getStaticSize() +
				(Object::OBJECT_CHILDREN_COUNT + obj->getObject()->getType()->getPropertiesCount()) * sizeof(Object*), BEER_WIDEN("object size is not right"));
		}
	}

	{
		Object** obj = reinterpret_cast<Object**>(mFrom->getMemory());

		while (mFrom->contains((byte*)obj))
		{
			if (mTo->contains((byte*)*obj))
			{
				GCObject* parent = reinterpret_cast<GCObject*>(mFrom->getMemory());

				while (reinterpret_cast<Object**>(parent) < obj && reinterpret_cast<Object**>(reinterpret_cast<byte*>(parent) + parent->getSize()) < obj)
				{
					parent = reinterpret_cast<GCObject*>(reinterpret_cast<byte*>(parent) + parent->getSize());
				}

				Trav trav(*obj);

				parent->getObject()->getType()->getTraverser()(&trav, parent->getObject()->getType(), parent->getObject());

				GCObject* child = reinterpret_cast<GCObject*>(mTo->getMemory());

				while (reinterpret_cast<Object*>(parent) < *obj && reinterpret_cast<Object*>(reinterpret_cast<byte*>(parent) + parent->getSize()) < *obj)
				{
					child = reinterpret_cast<GCObject*>(reinterpret_cast<byte*>(parent) + parent->getSize());
				}

				DBG_ASSERT(false, BEER_WIDEN("pointers from from-space into to-space"));
			}
			obj++;
		}
	}

#endif

	mTo->clear();
}