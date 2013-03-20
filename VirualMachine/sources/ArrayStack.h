#pragma once
#include "prereq.h"


namespace Beer
{
	template <typename T>
	class ArrayStack
	{
	protected:
		uint32 mSize;
		T* mItems;
		uint32 mNext;

	public:
		INLINE ArrayStack()
			: mSize(0), mItems(NULL), mNext(0)
		{
		}

		INLINE ArrayStack(T* memory, uint32 size)
			: mSize(size), mItems(memory), mNext(0)
		{
			clear();
		}

		INLINE ~ArrayStack()
		{
		}

		INLINE uint32 absoluteIndex(int32 index)
		{
			DBG_ASSERT(index < 0, BEER_WIDEN("Stack index out of bounds"));
			return static_cast<uint32>((static_cast<int64>(mNext) - 1 + index));
		}

		INLINE void clear()
		{
			mNext = 0;
		}

		INLINE uint32 push()
		{
			check(1);
			mItems[mNext] = NULL;
			return mNext++;
		}

		INLINE uint32 push(T obj)
		{
			check(1);
			mItems[mNext] = obj;
			return mNext++;
		}

		INLINE void pop()
		{
			DBG_ASSERT(mNext > 0, BEER_WIDEN("Attempting to pop empty stack"));
			mNext--;
		}

		INLINE T top()
		{
			DBG_ASSERT(mNext > 0, BEER_WIDEN("Stack index out of bounds"));
			return mItems[mNext - 1];
		}

		INLINE T top(uint32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Stack index out of bounds"));
			return mItems[index];
		}

		INLINE T* topPtr(uint32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Stack index out of bounds"));
			return &mItems[index];
		}

		INLINE T top(int32 index)
		{
			return top(absoluteIndex(index));
		}
		
		template <typename T>
		INLINE T top()
		{
			return static_cast<T>(top());
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

		INLINE void set(T obj, uint32 absoluteIndex)
		{
			DBG_ASSERT(absoluteIndex < mNext, BEER_WIDEN("Attempting to change value out of the stack"));
			mItems[absoluteIndex] = obj;
		}

		INLINE void set(T obj, int32 index)
		{
			set(obj, absoluteIndex(index));
		}

		INLINE uint32 topIndex()
		{
			return mNext - 1;
		}

		INLINE uint32 size()
		{
			return mNext;
		}

		INLINE uint32 realSize()
		{
			return mSize;
		}

		INLINE void move(int32 count) // must be signed !!!
		{
			check(count);
			mNext += count;

			if(count > 0)
			{
				memset(&mItems[mNext - count], 0, count * sizeof(T));
			}
		}

		INLINE void move(uint32 count) // shoult not be zero
		{
			DBG_ASSERT(count > 0, "Passed value is zero");
			check(count);
			mNext += count;
			memset(&mItems[mNext - count], 0, count * sizeof(T));
		}

		NOINLINE void check(int32 count) // must be signed !!!
		{
			if(mNext + count >= mSize)
			{
				throw StackOverflowException(BEER_WIDEN("Unable to use more stack"));
			}
		}

		NOINLINE bool check(uint32 count)
		{
			if(mNext + count >= mSize)
			{
				throw StackOverflowException(BEER_WIDEN("Unable to use more stack"));
			}
		}
	};
};