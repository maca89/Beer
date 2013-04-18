#include "stdafx.h"
#include "AllocationBlock.h"

#include "NurseryGC.h"

using namespace Beer;

void AllocationBlock::init()
{
	mMemory = mGC->alloc(mSize);
	mFilled = 0;
}

Object * AllocationBlock::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	Object * obj = FixedHeap::alloc(staticSize, childrenCount, preOffset);

	if (!obj)
	{
		uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount + sizeof(GCObject));
		
		if (size > mLargeObject)
		{
			obj = reinterpret_cast<Object*>(mGC->alloc(size));

			initObject(obj, staticSize, size);
		}
		else
		{
			init();
			obj = FixedHeap::alloc(staticSize, childrenCount, preOffset);
		}

		if (!obj) throw NotEnoughMemoryException(BEER_WIDEN("Cannot allocate"));
	}

	return obj;
}

byte* AllocationBlock::alloc(uint32 size)
{
	byte* obj = FixedHeap::alloc(size);

	if (!obj)
	{
		init();
		obj = FixedHeap::alloc(size);

		if (!obj) throw NotEnoughMemoryException(BEER_WIDEN("Cannot allocate"));
	}

	return obj;
}