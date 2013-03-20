#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "StackRef.h"


namespace Beer
{
	class VirtualMachine;

	class Pair : public Object
	{
	public:
		enum
		{
			CHILD_ID_PAIR_FIRST = OBJECT_CHILDREN_COUNT,
			CHILD_ID_PAIR_SECOND = OBJECT_CHILDREN_COUNT + 1,
		};

		static void BEER_CALL init(Thread* thread, StackRef<Pair> receiver, StackRef<Pair> ret);
		static void BEER_CALL init_ObjectObject(Thread* thread, StackRef<Pair> receiver, StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret);

		static void BEER_CALL getFirst(Thread* thread, StackRef<Pair> receiver, StackRef<Object> ret);
		static void BEER_CALL setFirst(Thread* thread, StackRef<Pair> receiver, StackRef<Object> value);
		
		static void BEER_CALL getSecond(Thread* thread, StackRef<Pair> receiver, StackRef<Object> ret);
		static void BEER_CALL setSecond(Thread* thread, StackRef<Pair> receiver, StackRef<Object> value);
	};

	class PairClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual void createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret);
		virtual void initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass);
	};

	// inlined methods
	
	INLINE void BEER_CALL Pair::init(Thread* thread, StackRef<Pair> receiver, StackRef<Pair> ret)
	{
		ret = receiver;
	}

	INLINE void BEER_CALL Pair::init_ObjectObject(Thread* thread, StackRef<Pair> receiver, StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret)
	{
		Pair::setFirst(thread, receiver, first);
		Pair::setSecond(thread, receiver, second);
		ret = receiver;
	}

	INLINE void BEER_CALL Pair::getFirst(Thread* thread, StackRef<Pair> receiver, StackRef<Object> ret)
	{
		Object::getChild(thread, receiver, CHILD_ID_PAIR_FIRST, ret);
	}

	INLINE void BEER_CALL Pair::setFirst(Thread* thread, StackRef<Pair> receiver, StackRef<Object> value)
	{
		Object::setChild(thread, receiver, CHILD_ID_PAIR_FIRST, value);
	}

	INLINE void BEER_CALL Pair::getSecond(Thread* thread, StackRef<Pair> receiver, StackRef<Object> ret)
	{
		Object::getChild(thread, receiver, CHILD_ID_PAIR_SECOND, ret);
	}

	INLINE void BEER_CALL Pair::setSecond(Thread* thread, StackRef<Pair> receiver, StackRef<Object> value)
	{
		Object::setChild(thread, receiver, CHILD_ID_PAIR_SECOND, value);
	}
};