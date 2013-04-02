#pragma once
#include "prereq.h"
#include "Pool.h"
#include "Integer.h"


namespace Beer
{
	class Method;
	class Class;
	class String;
	class Method;
	class Thread;

	class PolymorphicCache : public Pool
	{
	public:
		enum
		{
			POLYCACHE_CHILDREN_COUNT = POOL_CHILDREN_COUNT/* + 2*/,
			/*CHILD_ID_LOCKS = OBJECT_CHILDREN_COUNT,
			CHILD_ID_METHODS = OBJECT_CHILDREN_COUNT + 1*/
		};

	protected:
		String* mSelector;

	public:
		static void BEER_CALL init(Thread* thread, StackRef<PolymorphicCache> receiver, StackRef<String> selector);
		static void BEER_CALL clear(Thread* thread, StackRef<PolymorphicCache> receiver);
		static void BEER_CALL find(Thread* thread, StackRef<PolymorphicCache> receiver, StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret);
	};
};