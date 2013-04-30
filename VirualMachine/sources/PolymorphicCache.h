#pragma once
#include "prereq.h"
#include "Pool.h"
#include "Mutex.h"


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
			POLYCACHE_CHILDREN_COUNT = POOL_CHILDREN_COUNT,
		};

	protected:
		CriticalSection mCriticalSection;

	public:
		PolymorphicCache();
		void clear();
		Method* find(Class* receiverType, Class* interf, uint32 index);

	public:
		//static void BEER_CALL clear(Thread* thread, StackRef<PolymorphicCache> receiver);
		//static void BEER_CALL find(Thread* thread, StackRef<PolymorphicCache> receiver, StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret);
		
	};
};