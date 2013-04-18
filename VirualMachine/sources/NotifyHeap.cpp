#include "stdafx.h"
#include "NotifyHeap.h"

#include "prereq.h"
#include "Object.h"
#include "GCObject.h"

using namespace Beer;

NotifyHeap::NotifyHeap(size_t initSize, size_t threshold, HeapThresholdNotify* notify)
	:	mInitSize(initSize),
	mSize(initSize),
	mFilled(0),
	mThresholdReached(false),
	mThreshold(threshold),
	mNotify(notify)
{
}

NotifyHeap::~NotifyHeap()
{
	::free(mMemory);
}

void NotifyHeap::init()
{
	DBG_ASSERT(!mMemory, BEER_WIDEN("Init was already called"));

	mMemory = reinterpret_cast<byte*>(::malloc(mInitSize));
}

Object* NotifyHeap::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount + sizeof(GCObject));

	if (!canAlloc(size)) return NULL;

	Object* obj = reinterpret_cast<Object*>(mMemory + mFilled + sizeof(GCObject));

	mFilled += size;

	initObject(obj, staticSize, size);

	if (!mThresholdReached && mFilled > mThreshold)
	{
		mThresholdReached = true;

		if (mNotify) mNotify->thresholdReached(this, mThreshold);
	}

	return obj;
}

byte* NotifyHeap::alloc(uint32 size)
{
	size = roundSize(size);

	if (!canAlloc(size)) return NULL;

	byte* obj = mMemory + mFilled;

	mFilled += size;

	if (!mThresholdReached && mFilled > mThreshold)
	{
		mThresholdReached = true;

		if (mNotify) mNotify->thresholdReached(this, mThreshold);
	}

	return obj;
}

void NotifyHeap::clear()
{
	mFilled = 0;
	mThresholdReached = false;

#ifdef BEER_DEBUG_MODE	
	::memset(mMemory, 0, mSize);
#endif
}