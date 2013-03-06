#include "stdafx.h"
#include "NurseryGC.h"

#include "AlignedHeap.h"
#include "AllocationBlock.h"

using namespace Beer;

NurseryGC::NurseryGC(uint8 nurseryBitSize, size_t blockSize)
	:	mBlockSize(blockSize)
{
	mAlloc = new AlignedHeap(nurseryBitSize);
	mCollect = new AlignedHeap(nurseryBitSize);
	mPromote = new AlignedHeap(nurseryBitSize);

	mHeapSize = ((AlignedHeap*)mAlloc)->getSize();

	::InitializeCriticalSection(&mCS);
}

NurseryGC::~NurseryGC()
{
	delete mAlloc;
	delete mCollect;
	delete mPromote;

	::DeleteCriticalSection(&mCS);
}

void NurseryGC::init()
{
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