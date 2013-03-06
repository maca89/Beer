#include "stdafx.h"
#include "GenerationalGC.h"

#include "AllocationBlock.h"
#include "Heap.h"
#include "SimpleMemoryAllocator.h"

using namespace Beer;

GenerationalGC::GenerationalGC(unsigned char nurseryBitSize, size_t blockSize)
	:	mState(Inactive)
{
	mNurseryGC = new NurseryGC(nurseryBitSize, blockSize);
	
	mMature = new SimpleMemoryAllocator(1024 * 1024);
	mPermanent = new SimpleMemoryAllocator(1024 * 1024);

	mReferences.push_back(NULL); // 0 => NULL
	mReferencesNext = 1;

	InitializeCriticalSection(&mMatureCS);
	InitializeCriticalSection(&mPermanentCS);
}

GenerationalGC::~GenerationalGC()
{
	delete mNurseryGC;
	delete mMature;
	delete mPermanent;

	DeleteCriticalSection(&mMatureCS);
	DeleteCriticalSection(&mPermanentCS);
}

void GenerationalGC::init()
{
	mNurseryGC->init();
	mMature->init(1024 * 1024);
	mPermanent->init(1024 * 1024);
}

Object* GenerationalGC::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	EnterCriticalSection(&mMatureCS);

	LeaveCriticalSection(&mMatureCS);

	return NULL;
}

void GenerationalGC::collect()
{

}