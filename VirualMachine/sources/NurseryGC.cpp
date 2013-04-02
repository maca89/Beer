#include "stdafx.h"
#include "NurseryGC.h"

#include "AllocationBlock.h"
#include "MyDynamicHeap.h"
#include "VirtualMachine.h"

using namespace Beer;

NurseryGC::NurseryGC(size_t initSize, size_t blockSize)
	:	mVM(NULL),
		mBlockSize(blockSize)
{
	/*mAlloc = new NurseryHeap(initSize, static_cast<size_t>(initSize * 0.75f), this);
	mCollect = new NurseryHeap(initSize, static_cast<size_t>(initSize * 0.75f), this);
	mPromote = new NurseryHeap(initSize, static_cast<size_t>(initSize * 0.75f), this);*/

	mAlloc = new NurseryHeap(initSize);
	mCollect = new NurseryHeap(initSize);
	mPromote = new NurseryHeap(initSize);

	::InitializeCriticalSection(&mCS);
}

NurseryGC::~NurseryGC()
{
	delete mAlloc;
	delete mCollect;
	delete mPromote;

	::DeleteCriticalSection(&mCS);
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
	ThreadSet& threads = mVM->getThreads();

	/*for (ThreadSet::iterator it = threads.beginLocked(); it.hasNext(); it++)
	{
		(*it)->setSaveSuspend(true);
	}*/
}