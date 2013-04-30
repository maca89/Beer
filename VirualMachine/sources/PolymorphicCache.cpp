#include "stdafx.h"
#include "PolymorphicCache.h"
#include "Class.h"
#include "Method.h"
#include "Frame.h"
#include "Thread.h"
#include "Pair.h"

using namespace Beer;

Method* interfaceMethodLookup(Class* receiverType, Class* interf, uint32 methodIndex)
{
	uint32 interfIndex = 0;
	if(!receiverType->findInterfaceMethodsStart(interf, interfIndex))
	{
		throw Exception(BEER_WIDEN("Interface method not found")); // TODO: better exception
	}

	return receiverType->getMethod(interfIndex + methodIndex);
}

PolymorphicCache::PolymorphicCache() : mCriticalSection()
{
}

void PolymorphicCache::clear()
{
	for(uint32 i = 0; i < getLength(); i++)
	{
		setItem(i, NULL);
	}
}

Method* PolymorphicCache::find(Class* receiverType, Class* interf, uint32 methodIndex)
{
	//return interfaceMethodLookup(receiverType, interf, methodIndex);

	for(uint32 sloti = 0; sloti < getLength(); sloti += 2)
	{
		Object* key = getItem(sloti);
		
		// found!
		if(key == receiverType)
		{
			return static_cast<Method*>(getItem(sloti + 1));
		}

		// end of cache
		while(key == NULL)
		{
			NonBlockingMutex nbMutex(&mCriticalSection);

			// patch the cache!
			if(nbMutex.entered())
			{
				if(key == NULL) // could have been patched by someone else before we acquired the lock
				{
					Method* value = interfaceMethodLookup(receiverType, interf, methodIndex);

					setItem(sloti + 1, value); // first set value
					setItem(sloti, receiverType); // second set key
					return value; // releases nbMutex
				}
			}

			// umm, someone else is working with the cache (probably with this index, but we dont know)
			else
			{
				BlockingMutex blMutex(&mCriticalSection); // wait until they finish (blocking wait)
				key = getItem(sloti); // lets see if the slot is free
			}
		}
	}

	return interfaceMethodLookup(receiverType, interf, methodIndex);
}

/*void BEER_CALL PolymorphicCache::clear(Thread* thread, StackRef<PolymorphicCache> receiver)
{
	static_cast<PolymorphicCache*>(thread->getGC()->getIdentity(receiver))->clear();
}*/


/*void PolymorphicCache::find(Thread* thread, StackRef<PolymorphicCache> receiver, StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret)
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
	ret = klass->findVirtualMethod(*selector);
	//Class::findMethod(thread, klass, selector, ret);

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
}*/