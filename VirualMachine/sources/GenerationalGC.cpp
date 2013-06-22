#include "stdafx.h"
#include "GenerationalGC.h"
#include "VirtualMachine.h"
#include "CopyingGC.h"
#include "AllocationBlock.h"
#include "DynamicHeap.h"

using namespace Beer;

GenerationalGC::GenerationalGC(uint32 nurserySize, uint32 blockSize, uint32 matureSize, uint32 permanentSize)
	:	mState(INACTIVE)
{
	mNurseryGC = new NurseryGC(this, nurserySize, blockSize);	
	mMatureGC = new MatureGC(this, matureSize);

	mMemPermStart = new byte[permanentSize];
	mPermanent = new PermanentHeap(mMemPermStart, permanentSize);

	mReferences.push_back(NULL); // 0 => NULL
	mReferencesNext = 1;
}

GenerationalGC::~GenerationalGC()
{
	delete mNurseryGC;
	delete mMatureGC;
	delete mMemPermStart;
	delete mPermanent;
}

void GenerationalGC::init(VirtualMachine* vm)
{
	mVM = vm;
	mPermanent->init();
}

void GenerationalGC::nurseryFull()
{
	BlockingMutex mutex(&mStateMutex);

	if (mState == INACTIVE)
	{
		mState = NURSERY_COLL_PENDING;
		mVM->startSafePoint();
	}
}

void GenerationalGC::threadsSuspended()
{
	BlockingMutex mutex(&mStateMutex);

	switch (mState)
	{
		case NURSERY_COLL_PENDING:

#ifdef BEER_GC_STATS

			mStats.mNurseryCollections++;
#endif

			mState = NURSERY_COLL_RUNNING;
			mNurseryGC->collect(mVM->getScheduler(), &mRemSet);
			break;

		case MATURE_COLL_PENDING:

#ifdef BEER_GC_STATS

			mStats.mMatureCollections++;
#endif

			mState = MATURE_COLL_RUNNING;
			mMatureGC->collect(mVM->getScheduler(), &mRemSet);
			break;
	}
}

void GenerationalGC::restartThreads()
{
	mVM->stopSafePoint();
}

void GenerationalGC::afterCollection()
{
	switch (mState)
	{
		case NURSERY_COLL_RUNNING:
			mNurseryGC->afterCollection();
			mState = INACTIVE;
			break;

		case MATURE_COLL_RUNNING:
			mMatureGC->afterCollection();
			mState = INACTIVE;
			break;
	}
}