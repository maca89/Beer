#pragma once
#include "prereq.h"


namespace Beer
{
	template <typename T>
	class DynamicStack
	{
	protected:
		uint32 mSize;
		T* mItems;
		uint32 mNext;

	public:
		INLINE DynamicStack(uint32 initsize = 50) : mSize(initsize), mItems(NULL), mNext(0)
		{
			mItems = new T[mSize];
			memset(mItems, 0, mSize * sizeof(T));
			clear();
		}

		INLINE ~DynamicStack()
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
			check();
			if(obj == reinterpret_cast<T>(0x0e))
			{
				std::cout << "!";
			}
			mItems[mNext] = obj;
			return mNext++;
		}

		INLINE void pop()
		{
			mNext--;
		}

		INLINE T top(uint32 index)
		{
			T t = mItems[index];
			return t;
		}

		INLINE T top(int32 index)
		{
			return top(absoluteIndex(index));
		}

		INLINE void set(T obj, uint32 absoluteIndex)
		{
			DBG_ASSERT(absoluteIndex < mNext, "Attempting to change value out of the stack");
			/*if(obj == reinterpret_cast<T>(0x0e))
			{
				std::cout << "!";
			}*/
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

		INLINE uint32 topIndex()
		{
			return mNext - 1;
		}

		INLINE uint32 size()
		{
			return mNext;
		}

		INLINE void move(int16 count)
		{
			check(count);
			mNext += count;
			
			if(count > 0)
			{
				memset(&mItems[mNext - count], 0, count * sizeof(T));
			}
		}

		INLINE void check(int16 count = 1)
		{
			if(mNext + count >= mSize)
			{
				enlarge();
			}
		}

		NOINLINE void enlarge()
		{
			uint32 newSize = mSize * 10;
			T* newItems = new T[newSize];
			memcpy(newItems, mItems, mSize * sizeof(T));
			//memset(&newItems[mSize], 0, (newSize - mSize) * sizeof(T));
			SMART_DELETE_ARR(mItems);
			mItems = newItems;
			mSize = newSize;

			//std::cout << "// Stack enlarged" << std::endl;
		}
	};
};