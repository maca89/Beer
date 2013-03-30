#pragma once
#include "prereq.h"
#include "Object.h"
#include "StackRef.h"
#include "Thread.h"
#include "TraverseObjectReceiver.h"


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

		static void createInstance(Thread* thread, StackRef<Class> receiver, uint16 length, StackRef<Pool> ret);

		static void BEER_CALL clear(Thread* thread, StackRef<Pool> receiver);
		static void BEER_CALL copyFrom(Thread* thread, StackRef<Pool> receiver, StackRef<Pool> other);

		// traversers
		static void PoolInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance);
	};

	class PoolClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};

	INLINE void Pool::getLength(Thread* thread, StackRef<Pool> pool, uint16& length)
	{
		length = pool->mSize;
	}

	INLINE void Pool::setLength(Thread* thread, StackRef<Pool> pool, uint16 length)
	{
		pool->mSize = length;
	}

	INLINE void Pool::getItem(Thread* thread, StackRef<Pool> receiver, uint16 index, StackRef<Object> ret)
	{
		Object::getChild(thread, receiver, CHILD_ID_POOL_START + index, ret);
	}

	INLINE void Pool::setItem(Thread* thread, StackRef<Pool> receiver, uint16 index, StackRef<Object> item)
	{
		Object::setChild(thread, receiver, CHILD_ID_POOL_START + index, item);
	}

	INLINE void Pool::createSlot(Thread* thread, StackRef<Pool> receiver, uint16& ret)
	{
		ret = receiver->mNext++;
	}

	INLINE void Pool::hasFreeSlot(Thread* thread, StackRef<Pool> receiver, bool& ret)
	{
		ret = receiver->mNext < receiver->mSize;
	}
};