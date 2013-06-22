#include "stdafx.h"
#include "FixedHeap.h"

#include "Object.h"
#include "GCObject.h"

using namespace Beer;

FixedHeap::~FixedHeap()
{
}

void FixedHeap::clear()
{
	mFilled = 0;
	::memset(mMemStart, 0, mSize);
}

void FixedHeap::init()
{
	::memset(mMemStart, 0, mSize);
}

Object* FixedHeap::alloc(uint32 staticSize, uint32 childrenCount)
{
	uint32 size = calcSize(staticSize, childrenCount);

	if (!canAlloc(size))
	{
		return NULL;
	}

	Object* obj = reinterpret_cast<Object*>(mMemStart + mFilled + sizeof(GCObject));

	mFilled += size;

	initObject(obj, size, staticSize);
	
	return obj;
}

byte* FixedHeap::alloc(size_t size)
{
	if (!canAlloc(size))
	{
		return NULL;
	}

	byte* obj = mMemStart + mFilled;

	mFilled += size;

	return obj;
}