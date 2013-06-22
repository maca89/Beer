#pragma once

#include "Heap.h"

namespace Beer
{
	class HeapThresholdNotify
	{
		public:

			virtual void thresholdReached(Heap* heap, size_t treshold) = 0;
	};

	class NotifyHeap : public Heap
	{
	protected:

		size_t mInitSize;
		size_t mSize;
		size_t mFilled;

		byte* mMemory;

		size_t mThreshold;
		bool mThresholdReached;
		HeapThresholdNotify* mNotify;

	public:

		NotifyHeap(size_t initSize, size_t threshold = -1, HeapThresholdNotify* notify = NULL);

		virtual ~NotifyHeap();

		virtual void init();

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount);
		virtual byte* alloc(uint32 size);

		void clear();

		//virtual void free(Object* obj);

		INLINE bool contains(byte* ptr)
		{
			return mMemory <= ptr && (mMemory + mSize) >= ptr;
		}

	protected:

		INLINE bool canAlloc(size_t size)
		{
			return mFilled + size <= mSize;
		}
	};
};