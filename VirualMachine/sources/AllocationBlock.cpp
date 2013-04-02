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
		init();
		obj = FixedHeap::alloc(staticSize, childrenCount, preOffset);
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
	}

	return obj;
}