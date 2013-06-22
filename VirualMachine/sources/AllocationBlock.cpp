#include "stdafx.h"
#include "AllocationBlock.h"
#include "NurseryGC.h"

#include "GenerationalGC.h"

using namespace Beer;

void AllocationBlock::init()
{
	mMemory = mGC->allocHeap(mSize);
	mFilled = mMemory ? 0 : mSize;
}

Object * AllocationBlock::alloc(uint32 staticSize, uint32 childrenCount)
{
	Object* obj = NULL;

	uint32 size = calcSize(staticSize, childrenCount);

	if (isLargeObject(size))
	{
		obj = reinterpret_cast<Object*>(mGC->alloc(size) + sizeof(GCObject));
	}
	else
	{
		obj = reinterpret_cast<Object*>(alloc(size) + sizeof(GCObject));
	}

	//obj = reinterpret_cast<Object*>(alloc(size) + sizeof(GCObject));
	
	initObject(obj, size, staticSize);
	
	return obj;
}

byte* AllocationBlock::alloc(uint32 size)
{
	//return new byte[size];

	if (!canAlloc(size))
	{
		init();

		if (!canAlloc(size))
		{
			return mGC->alloc(size);
		}
	}

#ifdef BEER_GC_STATS
	mGC->getGenerationalGC()->mStats.mAllocatedNursery += size;
#endif

	byte* obj = mMemory + mFilled;
	
	mFilled += size;

	return obj;
}

void AllocationBlock::invalidate()
{
	mFilled = mSize;
}