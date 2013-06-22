/*#pragma once
#include "prereq.h"
#include "FixedHeap.h"

namespace Beer
{
	class AlignedHeap : public FixedHeap
	{
	protected:

		uint8 mBitSize;

	public:

		AlignedHeap(uint8 bitSize);
		virtual ~AlignedHeap();

		virtual void init();

		INLINE bool contains(void * pointer)
		{
			size_t tmp = reinterpret_cast<size_t>(mMemory) ^ reinterpret_cast<size_t>(pointer);
			tmp >>= mBitSize;
			return !pointer || tmp == 0;
		}
	};
}*/