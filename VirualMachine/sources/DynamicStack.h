#pragma once
#include "prereq.h"
#include "GrowableMemoryAllocator.h"


namespace Beer
{
	template <typename T>
	class DynamicStack
	{
	protected:
		uint32 mSize;
		T* mItems;
		uint32 mNext;
		GrowableMemoryAllocator mAllocator;

	public:
#ifdef BEER_DEBUG_MODE
		INLINE DynamicStack(uint32 commitSize = 1000, uint32 reserveSize = 2000) // default commit ~4KB, default reserve ~8KB
#else
		INLINE DynamicStack(uint32 commitSize = 1000, uint32 reserveSize = 260000) // default commit ~4KB, default reserve ~1MB
#endif
			: mSize(0), mItems(NULL), mNext(0), mAllocator()
		{
			mItems = reinterpret_cast<T*>(mAllocator.malloc(commitSize * sizeof(T), reserveSize * sizeof(T)));
			mSize = reserveSize;
			//memset(mItems, 0, mSize * sizeof(T));
			clear();
		}

		INLINE ~DynamicStack()
		{
			mAllocator.free(mItems);
		}

		INLINE uint32 absoluteIndex(int32 index)
		{
			return static_cast<uint32>((static_cast<int64>(mNext) - 1 + index) % mSize);
		}

		INLINE void clear()
		{
			mNext = 0;
		}

		INLINE uint32 push(T obj)
		{
			//check(1);
			mItems[mNext] = obj;
			return mNext++;
		}

		INLINE void pop()
		{
			mNext--;
		}

		INLINE T top(uint32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Stack index out of bounds"));
			T obj = mItems[index];
			return obj;
		}

		INLINE T* topPtr(uint32 index)
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Stack index out of bounds"));
			T* obj = &mItems[index];
			return obj;
		}

		INLINE T top(int32 index)
		{
			return top(absoluteIndex(index));
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

		INLINE void move(int32 count) // must be signed !!!
		{
			//check(count);
			mNext += count;
			
			if(count > 0)
			{
				memset(&mItems[mNext - count], 0, count * sizeof(T));
			}
		}

		INLINE void move(uint32 count) // shoult not be zero
		{
			DBG_ASSERT(count > 0, "Passed value is zero");
			//check(count);
			mNext += count;
			memset(&mItems[mNext - count], 0, count * sizeof(T));
		}

		INLINE void check(int32 count) // must be signed !!!
		{
			if(mNext + count >= mSize)
			{
				throw StackOverflowException(BEER_WIDEN("Unable to use more stack"));
				//enlarge();
			}
		}

		/*NOINLINE void enlarge()
		{
			uint32 newSize = mSize * 10;
			T* newItems = new T[newSize];
			memcpy(newItems, mItems, mSize * sizeof(T));
			//memset(&newItems[mSize], 0, (newSize - mSize) * sizeof(T));
			SMART_DELETE_ARR(mItems);
			mItems = newItems;
			mSize = newSize;
			//cout << "[DynamicStack::enlarge]";
		}*/
	};
};