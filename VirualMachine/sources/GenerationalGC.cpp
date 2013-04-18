#include "stdafx.h"
#include "GenerationalGC.h"

#include "AllocationBlock.h"
#include "DynamicHeap.h"

using namespace Beer;

GenerationalGC::GenerationalGC(size_t nurserySize, size_t blockSize)
	:	mState(GC_ALLOC)
{
	mNurseryGC = new NurseryGC(this, nurserySize, blockSize);
	
	mMature = new MatureHeap(1024 * 1024);
	mPermanent = new PermanentHeap(1024 * 1024);

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

void GenerationalGC::init(VirtualMachine* vm)
{
	mNurseryGC->init(vm);
	mMature->init();
	mPermanent->init();
}

Object* GenerationalGC::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	EnterCriticalSection(&mMatureCS);
	
	Object* obj = mMature->alloc(staticSize, childrenCount, preOffset);

	LeaveCriticalSection(&mMatureCS);

	return obj;
}

void GenerationalGC::collect()
{

}