#include "stdafx.h"
#include "MatureGC.h"
#include "FixedHeap.h"
#include "GenerationalGC.h"

using namespace Beer;

MatureGC::MatureGC(GenerationalGC* gc, uint32 size)
{
	mGC = gc;

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

	//if (!obj) mGC->nurseryFull();

	return obj;
}

void MatureGC::collect(TaskScheduler* scheduler, RememberedSet* remSet)
{
}

void MatureGC::afterCollection()
{
}