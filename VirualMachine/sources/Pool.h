#pragma once
#include "prereq.h"
#include "Object.h"
#include "StackRef.h"


namespace Beer
{
	class Pool : public Object
	{
	public:
		enum
		{
			POOL_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT,
			CHILD_ID_POOL_START = OBJECT_CHILDREN_COUNT
		};

	protected:
		uint16 mSize;
		uint16 mNext;

	public:
		INLINE void setSize(uint16 value)
		{
			mSize = value;
		}
		
		INLINE uint16 getSize() const
		{
			return mSize;
		}

		INLINE void setNext(uint16 value)
		{
			mNext = value;
		}
		
		INLINE uint16 getNext() const
		{
			return mNext;
		}

		INLINE bool hasFreeSlot() const
		{
			return mNext < mSize;
		}

		INLINE uint16 createSlot()
		{
			return mNext++;
		}

		INLINE Object* getItem(uint16 index) const
		{
			return mChildren[CHILD_ID_POOL_START + index];
		}

		INLINE void setItem(uint16 index, Object* item)
		{
			mChildren[CHILD_ID_POOL_START + index] = item;
		}

		INLINE uint16 addItem(Object* item)
		{
			mChildren[CHILD_ID_POOL_START + mNext] = item;
			return mNext++;
		}

		NOINLINE bool find(Object* item, uint16& out_index)
		{
			for(uint16 i = 0; i < mSize; i++)
			{
				if(getItem(i) == item)
				{
					out_index = i;
					return true;
				}
			}

			return false;
		}

		NOINLINE void copyFrom(Pool* p2)
		{
			uint16 min = mSize < p2->getSize() ? mSize : p2->getSize();
			for(uint16 i = 0; i < min; i++)
			{
				setItem(i, p2->getItem(i));
			}
			mNext = p2->mNext;
		}

		NOINLINE void clear()
		{
			for(uint16 i = 0; i < mSize; i++)
			{
				setItem(i, NULL);
			}
			mNext = 0;
		}

		INLINE static void getItem(Thread* thread, Pool* pool, uint16 index, StackRef<Object> ret)
		{
			ret = pool->getItem(index);
		}

		INLINE static void setItem(Thread* thread, Pool* pool, uint16 index, StackRef<Object> item)
		{
			pool->setItem(index, *item);
		}

		INLINE static void addItem(Thread* thread, Pool* pool, StackRef<Object> item, uint16& ret)
		{
			ret = pool->addItem(*item);
		}

		INLINE static void hasFreeSlot(Thread* thread, Pool* pool, bool& ret)
		{
			ret = pool->hasFreeSlot();
		}

		INLINE static void find(Thread* thread, Pool* pool, StackRef<Object> item, bool& ret1, uint16& ret2)
		{
			ret1 = pool->find(*item, ret2);
		}
	};
};