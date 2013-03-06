#include "stdafx.h"
#include "AlignedHeap.h"

//#include <Windows.h>

using namespace Beer;

AlignedHeap::AlignedHeap(unsigned char bitSize)
	:	FixedHeap(2 << (bitSize - 1))
{
	mBitShift = bitSize - 1;
}

AlignedHeap::~AlignedHeap()
{
	::_aligned_free(mMemory);
}

void AlignedHeap::init()
{
	mMemory = ::_aligned_malloc(mSize, 2 >> mBitShift);

#ifdef BEER_MEMORY_DEBUGGING
	ZeroMemory(mMemory, mSize);
#endif
}