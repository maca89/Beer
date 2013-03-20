#pragma once
#include "prereq.h"


namespace Beer
{
	template <typename T>
	class DownwardStack
	{
	protected:
		int32 mSize;
		int32 mNext;
		T* mBp;

	public:
		INLINE DownwardStack(void* bp, int32 size)
			: mSize(size), mBp(reinterpret_cast<T*>(bp)), mNext(0)
		{
		}

		INLINE ~DownwardStack()
		{
		}

		INLINE int32 push()
		{
			check(1);
			*(sp() - 1) = NULL;
			return mNext++;
		}

		INLINE int32 push(T obj)
		{
			check(1);
			*(sp() - 1) = obj;
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
			return *sp();
		}

		INLINE T top(int32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Stack index out of bounds"));
			return *(bp() - realIndex(index));
		}

		INLINE T* topPtr(int32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Stack index out of bounds"));
			return (bp() - realIndex(index));
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

		INLINE void set(T obj, int32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Attempting to change value out of the stack"));
			*(bp() - realIndex(index)) = obj;
		}

		INLINE int32 topIndex()
		{
			return mNext - 1;
		}

		INLINE uint32 length()
		{
			return mNext;
		}

		INLINE int32 realLength()
		{
			return mSize;
		}

		INLINE T* bp()
		{
			return mBp;
		}

		INLINE T* sp()
		{
			return (bp() - mNext);
		}

		INLINE void move(int32 count) // must be signed !!!
		{
			//cout << "stack_move " << count << "\n";
			check(count);
			mNext += count;

			if(count > 0)
			{
				memset(sp(), 0, count * sizeof(T));
			}
		}

		NOINLINE void check(int32 count) // must be signed !!!
		{
			if(mNext + count >= mSize)
			{
				throw StackOverflowException(BEER_WIDEN("Unable to use more stack"));
			}
		}

	protected:
		INLINE int32 realIndex(int32 index)
		{
			return static_cast<int32>(index) + 1;
		}
	};
};