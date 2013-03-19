#pragma once
#include "prereq.h"
#include "Object.h"
#include "StackRef.h"


namespace Beer
{
	class Thread;

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
		static void getLength(Thread* thread, StackRef<Pool> pool, uint16& length);
		static void setLength(Thread* thread, StackRef<Pool> pool, uint16 length);

		static void getItem(Thread* thread, StackRef<Pool> pool, uint16 index, StackRef<Object> ret);
		static void setItem(Thread* thread, StackRef<Pool> pool, uint16 index, StackRef<Object> item);

		static void createSlot(Thread* thread, StackRef<Pool> receiver, uint16& ret);
		static void hasFreeSlot(Thread* thread, StackRef<Pool> pool, bool& ret);
		static void find(Thread* thread, StackRef<Pool> pool, StackRef<Object> item, bool& ret1, uint16& ret2);

		static void BEER_CALL clear(Thread* thread, StackRef<Pool> receiver);
		static void BEER_CALL copyFrom(Thread* thread, StackRef<Pool> receiver, StackRef<Pool> other);
	};
};