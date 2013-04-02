#pragma once

#include "Heap.h"

namespace Beer
{
	class FixedHeap : public Heap
	{
	protected:

		size_t mSize;
		byte* mMemory;
		size_t mFilled;

	public:

		INLINE FixedHeap(size_t size)
			:	mSize(size),
				mMemory(NULL),
				mFilled(0)
		{ }

		virtual ~FixedHeap();

		INLINE size_t getSize() const
		{
			return mSize;
		}

		virtual void init();
		
		virtual Object* alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset = 0);
		virtual byte* alloc(uint32 size);

		//virtual void free(Object* obj);

	protected:

		INLINE bool canAlloc(size_t size)
		{
			return mFilled + size <= mSize;
		}
	};
}