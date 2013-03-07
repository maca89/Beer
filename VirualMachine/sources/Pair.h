#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"


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
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};