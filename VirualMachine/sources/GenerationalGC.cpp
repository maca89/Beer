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
	mMatureGC = new MatureGC(this, matureSize, mNurseryGC);

	mMemPermStart = new byte[permanentSize];
	mPermanent = new PermanentHeap(mMemPermStart, permanentSize);

	mRemSet = new RememberedSet();

	mReferences.push_back(NULL); // 0 => NULL
	mReferencesNext = 1;
}

GenerationalGC::~GenerationalGC()
{
	delete mNurseryGC;
	delete mMatureGC;
	delete mMemPermStart;
	delete mPermanent;
	delete mRemSet;
}

void GenerationalGC::init(VirtualMachine* vm)
{
	mVM = vm;
	mPermanent->init();
}

void GenerationalGC::startCollection(Generation gen)
{
	BlockingMutex mutex(&mStateMutex);

	if (mState == INACTIVE)
	{
		switch (gen)
		{
			case GEN_NURSERY:
				if (mMatureGC->needCollect())
				{
					mState = MATURE_COLL_PENDING;
				}
				else
				{
					mState = NURSERY_COLL_PENDING;
				}
				mVM->startSafePoint();
				break;

			case GEN_MATURE:
				mState = MATURE_COLL_PENDING;
				mVM->startSafePoint();
				break;
		}		
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
			mNurseryGC->collect(mVM->getScheduler(), mRemSet);
			break;

		case MATURE_COLL_PENDING:

#ifdef BEER_GC_STATS

			mStats.mMatureCollections++;
#endif

			mState = MATURE_COLL_RUNNING;
			mMatureGC->collect(mVM->getScheduler(), mRemSet);
			break;
	}
}

void GenerationalGC::stopCollection(Generation gen)
{
	RememberedSet* temp = NULL;

	switch (gen)
	{
		case GEN_NURSERY:
			temp = mRemSet->forwardObjects();
			break;

		case GEN_MATURE:
			temp = mRemSet->forwardSet();
			break;
	}

	delete mRemSet;
	mRemSet = temp;

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