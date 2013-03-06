#pragma once

#include "prereq.h"
#include "FixedHeap.h"

namespace Beer
{
	class NurseryGC;

	class AllocationBlock : public FixedHeap
	{
	protected:

		NurseryGC * mGC;

	public:
		INLINE AllocationBlock(size_t size, NurseryGC * gc)
			:	FixedHeap(size),
				mGC(gc)
		{ }
		
		virtual void init();

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset);

	protected:
		
		
	};
}