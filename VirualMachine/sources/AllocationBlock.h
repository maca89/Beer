#pragma once

#include "Heap.h"

namespace Beer
{
	class NurseryGC;

	class AllocationBlock : public Heap
	{
	protected:

		NurseryGC * mGC;

		uint32 mSize;
		uint32 mFilled;

		byte* mMemory;

		const uint32 mLargeObject;

	public:
		INLINE AllocationBlock(NurseryGC* gc, uint32 size)
			:	Heap(),
				mGC(gc),
				mSize(size),
				mFilled(size),
				mMemory(NULL),
				mLargeObject(static_cast<uint32>(size * 0.25f))
		{ }

		INLINE AllocationBlock(NurseryGC* gc, uint32 size, uint32 largeObject)
			:	Heap(),
				mGC(gc),
				mSize(size),
				mFilled(size),
				mMemory(NULL),
				mLargeObject(largeObject)
		{ }
		
		virtual void init();

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount);
		virtual byte* alloc(uint32 size);

		void invalidate();

	protected:

		INLINE bool isLargeObject(uint32 size)
		{
			return size > mLargeObject;
		}

		INLINE bool canAlloc(uint32 size)
		{
			return mFilled + size <= mSize;
		}
	};
}