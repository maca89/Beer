#pragma once

#include "prereq.h"
#include "FixedHeap.h"

namespace Beer
{
	class NurseryGC;

	class AllocationBlock : public FixedHeap
	{
	protected:

		const uint32 mLargeObject;
		NurseryGC * mGC;

	public:
		INLINE AllocationBlock(size_t size, NurseryGC * gc)
			:	FixedHeap(size),
				mLargeObject(static_cast<uint32>(0.5f * size)),
				mGC(gc)
		{ }
		
		virtual void init();

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset);
		virtual byte* alloc(uint32 size);

	protected:
		
		
	};
}