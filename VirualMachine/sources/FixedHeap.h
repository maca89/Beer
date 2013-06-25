#pragma once

#include "Heap.h"

namespace Beer
{
	class FixedHeap : public Heap
	{
	protected:

		uint32 mSize;
		uint32 mFilled;

		byte* mMemStart;
		byte* mMemEnd;

	public:

		INLINE FixedHeap(byte* memStart, uint32 size)
			:	Heap(),
				mSize(size),				
				mFilled(0),
				mMemStart(memStart),
				mMemEnd(memStart + size)
		{ }

		virtual ~FixedHeap();

		INLINE uint32 getSize() const
		{
			return mSize;
		}

		INLINE bool contains(void* ptr) const
		{
			return ptr >= mMemStart && ptr < mMemEnd;
		}

		INLINE byte* getMemory() const
		{
			return mMemStart;
		}

		INLINE uint32 getFilled() const
		{
			return mFilled;
		}

		void clear();

		virtual void init();
		
		virtual Object* alloc(uint32 staticSize, uint32 childrenCount);
		virtual byte* alloc(uint32 size);

		INLINE GCObject* firstObject()
		{
			return reinterpret_cast<GCObject*>(mMemStart);
		}

		INLINE bool allocated(void* obj)
		{
			return reinterpret_cast<byte*>(obj) >= mMemStart && reinterpret_cast<byte*>(obj) < mMemStart + mFilled;
		}

	protected:

		INLINE bool canAlloc(uint32 size)
		{
			return mFilled + size <= mSize;
		}
	};
}