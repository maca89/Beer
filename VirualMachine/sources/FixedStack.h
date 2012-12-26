#pragma once
#include "prereq.h"


namespace Beer
{
	//struct CollectedObject;

	template <typename T>
	class FixedStack
	{
	protected:
		uint32 mSize;
		T* mItems;
		uint32 mNext;

	public:
		INLINE FixedStack(uint32 size = 50) : mSize(size), mItems(NULL), mNext(0)
		{
			mItems = new T[mSize];
			clear();
		}

		INLINE ~FixedStack()
		{
			SMART_DELETE_ARR(mItems);
		}

		INLINE uint32 absoluteIndex(int32 index)
		{
			return static_cast<uint32>((static_cast<int64>(mNext) - 1 + index) % mSize);
		}

		NOINLINE void clear()
		{
			mNext = 0;
		}

		INLINE uint32 push(T obj)
		{
			mItems[mNext] = obj;
			return mNext++;
		}

		INLINE void pop()
		{
			mNext--;
		}

		INLINE T top(uint32 index)
		{
			return mItems[index];
		}

		INLINE T top(int32 index)
		{
			return top(absoluteIndex(index));
		}

		INLINE void set(T obj, uint32 absoluteIndex)
		{
			DBG_ASSERT(absoluteIndex < mNext, "Attempting to change value out of the stack");
			mItems[absoluteIndex] = obj;
		}

		INLINE void set(T obj, int32 index)
		{
			set(obj, absoluteIndex(index));
		}

		template <typename T>
		INLINE T top(int32 index)
		{
			return static_cast<T>(top(index));
		}

		template <typename T>
		INLINE T top(uint32 index)
		{
			return static_cast<T>(top(index));
		}

		INLINE uint32 size()
		{
			return mNext;
		}

		INLINE void move(int16 count)
		{
			mNext += count;
		}
	};
};