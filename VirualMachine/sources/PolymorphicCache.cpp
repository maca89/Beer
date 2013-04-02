#include "stdafx.h"
#include "PolymorphicCache.h"
#include "Class.h"
#include "Method.h"
#include "Frame.h"
#include "Thread.h"

using namespace Beer;

void BEER_CALL PolymorphicCache::init(Thread* thread, StackRef<PolymorphicCache> receiver, StackRef<String> selector)
{
	//thread->getGC()->setChild(receiver, NULL, selector); // TODO
	receiver->mSelector = *selector;
}

void BEER_CALL PolymorphicCache::clear(Thread* thread, StackRef<PolymorphicCache> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	uint16 length = 0;
	Pool::getLength(thread, receiver.staticCast<Pool>(), length);
	
	StackRef<Object> null(frame, frame->stackPush());

	for(uint32 i = 0; i < length; i++)
	{
		Pool::setItem(thread, receiver.staticCast<Pool>(), i, null);
	}
	
	frame->stackPop(); // null
}


void PolymorphicCache::find(Thread* thread, StackRef<PolymorphicCache> receiver, StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	uint16 length = 0;
	Pool::getLength(thread, receiver.staticCast<Pool>(), length);

	// search the cache
	{
		StackRef<Class> cachedClass(frame, frame->stackPush());
		StackRef<Pair> pair(frame, frame->stackPush());

		for(uint32 i = 0; i < length; i++)
		{
			Pool::getItem(thread, receiver.staticCast<Pool>(), i, pair);

			if(pair.isNull())
			{
				// end of cached block
				break;
			}

			Pair::getFirst(thread, pair, cachedClass);

			if(*cachedClass == *klass)
			{
				// we found cached method
				Pair::getSecond(thread, pair, ret);
				frame->stackMoveTop(-2); // pop cachedClass, pair
				return;
			}
		}

		frame->stackMoveTop(-2); // pop cachedClass, pair
	}

	// we must do a lookup
	Class::findMethod(thread, klass, selector, ret);

	// save method to cache
	if(!ret.isNull() && length > 0)
	{
		StackRef<Pair> pair(frame, frame->stackPush());

		// move all cached and pop the last one
		for(uint32 i = length; i > 1; i--) // *NOT* >=1 !!!
		{
			// pool[i - 1] = pool[i - 2];
			Pool::getItem(thread, receiver.staticCast<Pool>(), i - 2, pair);
			Pool::setItem(thread, receiver.staticCast<Pool>(), i - 1, pair);
		}

		// save at the top
		thread->createPair(klass, ret, pair);
		Pool::setItem(thread, receiver.staticCast<Pool>(), 0, pair);

		frame->stackPop(); // pop pair
	}
}