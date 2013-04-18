#include "stdafx.h"
#include "NurseryGC.h"

#include "AllocationBlock.h"
#include "VirtualMachine.h"
#include "GCObjectTraverser.h"

using namespace Beer;

NurseryGC::NurseryGC(GenerationalGC* gc, size_t initSize, size_t blockSize)
	:	mGC(gc),
	mVM(NULL),
	mBlockSize(blockSize),
	mCollectionCount(0)
{
	mAlloc = new NurseryHeap(initSize, static_cast<size_t>(initSize * 0.05f), this);
	mCollect = new NurseryHeap(initSize, static_cast<size_t>(initSize * 0.05f), this);
	mPromote = new NurseryHeap(initSize, static_cast<size_t>(initSize * 0.05f), this);

	::InitializeCriticalSection(&mCS);
	mThreadsSuspendedEvent = ::CreateEvent(NULL, false, false, NULL);

	if (!mThreadsSuspendedEvent)
	{
		throw GCException(BEER_WIDEN("Could not create event object"));
	}

	run();
}

NurseryGC::~NurseryGC()
{
	delete mAlloc;
	delete mCollect;
	delete mPromote;

	::DeleteCriticalSection(&mCS);
	::CloseHandle(mThreadsSuspendedEvent);
}

void NurseryGC::init(VirtualMachine* vm)
{
	mVM = vm;

	mAlloc->init();
	mCollect->init();
	mPromote->init();
}

Heap* NurseryGC::createHeap()
{
	AllocationBlock * block = new AllocationBlock(mBlockSize, this);
	block->init();
	return block;
}

void NurseryGC::thresholdReached(Heap* heap, size_t threshold)
{
	mVM->startSafePoint();
}

void NurseryGC::threadsSuspended()
{
	::SetEvent(mThreadsSuspendedEvent);
}

void NurseryGC::work()
{
	while (true)
	{
		DWORD res = ::WaitForSingleObject(mThreadsSuspendedEvent, INFINITE);

		if (res == WAIT_OBJECT_0)
		{
			switchHeaps();

			GCObjectTraverser traverser(mCollect, mGC->getMatureHeap());
			traverser.traverseObjects(mVM->getScheduler());

			mVM->stopSafePoint();

			mCollectionCount++;
		}
		else
		{
			throw GCException(BEER_WIDEN("Waiting for thread suspended event failed"));
		}
	}
}