#include "stdafx.h"
#include "FixedHeap.h"

#include "Object.h"
#include "GCObject.h"

using namespace Beer;

FixedHeap::~FixedHeap()
{
	::free(mMemory);
}

void FixedHeap::init()
{
	mMemory = reinterpret_cast<byte*>(::malloc(mSize));
}

Object* FixedHeap::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount/* + sizeof(GCObject)*/);

	if (!canAlloc(size)) return NULL;

	Object* obj = reinterpret_cast<Object*>(mMemory + mFilled/* + sizeof(GCObject)*/);

	mFilled += size;

	initObject(obj, staticSize, size);
	
	return obj;
}

byte* FixedHeap::alloc(size_t size)
{
	if (!canAlloc(size)) return NULL;

	byte* obj = mMemory + mFilled;

	mFilled += size;

	return obj;
}