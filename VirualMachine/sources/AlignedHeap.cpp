#include "stdafx.h"
/*#include "AlignedHeap.h"

//#include <Windows.h>

using namespace Beer;

AlignedHeap::AlignedHeap(unsigned char bitSize)
	:	FixedHeap(1 << bitSize, FixedHeap::MATURE_HEAP),
		mBitSize(bitSize)
{
}

AlignedHeap::~AlignedHeap()
{
	::_aligned_free(mMemory);
}

void AlignedHeap::init()
{
	mMemory = reinterpret_cast<byte*>(::_aligned_malloc(mSize, 1 >> mBitSize));

#ifdef BEER_MEMORY_DEBUGGING
	ZeroMemory(mMemory, mSize);
#endif
}*/