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
		uint16 getLength() const;
		void setLength(uint16 value);

		Object* getItem(uint16 index);
		void setItem(uint16 index, Object* item);

		uint32 createSlot();
		bool hasFreeSlot() const;

		//

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

	// inline definitions

	INLINE uint16 Pool::getLength() const
	{
		return mSize;
	}

	INLINE void Pool::setLength(uint16 value)
	{
		mSize = value;
	}

	INLINE Object* Pool::getItem(uint16 index)
	{
		return getChild(CHILD_ID_POOL_START + index);
	}

	INLINE void Pool::setItem(uint16 index, Object* item)
	{
		setChild(CHILD_ID_POOL_START + index, item);
	}

	INLINE uint32 Pool::createSlot()
	{
		return mNext++;
	}

	INLINE bool Pool::hasFreeSlot() const
	{
		return mNext < mSize;
	}

	//

	INLINE void Pool::getLength(Thread* thread, StackRef<Pool> pool, uint16& length)
	{
		length = static_cast<Pool*>(thread->getGC()->getIdentity(pool))->getLength();
	}

	INLINE void Pool::setLength(Thread* thread, StackRef<Pool> pool, uint16 length)
	{
		static_cast<Pool*>(thread->getGC()->getIdentity(pool))->setLength(length);
	}

	INLINE void Pool::getItem(Thread* thread, StackRef<Pool> pool, uint16 index, StackRef<Object> ret)
	{
		ret = static_cast<Pool*>(thread->getGC()->getIdentity(pool))->getItem(index);
	}

	INLINE void Pool::setItem(Thread* thread, StackRef<Pool> pool, uint16 index, StackRef<Object> item)
	{
		static_cast<Pool*>(thread->getGC()->getIdentity(pool))->setItem(index, *item);
	}

	INLINE void Pool::createSlot(Thread* thread, StackRef<Pool> pool, uint16& ret)
	{
		ret = static_cast<Pool*>(thread->getGC()->getIdentity(pool))->createSlot();
	}

	INLINE void Pool::hasFreeSlot(Thread* thread, StackRef<Pool> pool, bool& ret)
	{
		ret = static_cast<Pool*>(thread->getGC()->getIdentity(pool))->hasFreeSlot();
	}
};