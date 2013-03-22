#include "stdafx.h"
#include "Method.h"
#include "Object.h"
#include "GenerationalGC.h"
#include "Thread.h"
#include "Frame.h"
#include "Class.h"

using namespace Beer;



void Method::loadFromPool(Thread* thread, uint16 index, StackRef<Object> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Pool> pool(frame, frame->stackPush(
		mPool
	));

	Pool::getItem(thread, pool, index, ret);

	frame->stackPop(); // pop pool
}

uint16 Method::storeToPool(Thread* thread, StackRef<Object> object)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Pool> pool(frame, frame->stackPush(
		mPool
	));

	if(pool.isNull())
	{
		createPool(thread, 10);
		pool = mPool;
	}
	else
	{
		bool hasFreeSlot = false;
		Pool::hasFreeSlot(thread, pool, hasFreeSlot);

		if(!hasFreeSlot)
		{
			uint16 length = 0;
			Pool::getLength(thread, pool, length);
			createPool(thread, 2 * length);
			pool = mPool;
		}
	}

	uint16 index = 0;
	Pool::createSlot(thread, pool, index);
	Pool::setItem(thread, pool, index, object);
	
	frame->stackPop(); // pop pool
	return index;
}

void Method::updateAtPool(Thread* thread, uint16 index, StackRef<Object> object)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Pool> pool(frame, frame->stackPush(
		mPool
	));

	Pool::setItem(thread, pool, index, object);
	frame->stackPop(); // pop pool
}

void Method::createPool(Thread* thread, uint16 length)
{
	// TODO: get from VM's pool of pools
	
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Pool> newPool(frame, frame->stackPush());
	thread->createPool(newPool, length);

	if(mPool)
	{
		StackRef<Pool> oldPool(frame, frame->stackPush(
			mPool
		));

		Pool::copyFrom(thread, newPool, oldPool);
		mPool = *newPool;
		Pool::clear(thread, oldPool);

		frame->stackPop(); // pop oldPool
	}
	else
	{
		mPool = *newPool;
	}

	frame->stackPop(); // pop newPool
}


void Method::invokeNative(Thread* thread)
{
	Frame* frame = thread->getFrame();
	//BEER_STACK_CHECK();
	
	Method* invokedMethod = frame->stackTop<Method>(-1);
	DBG_ASSERT(invokedMethod != NULL, BEER_WIDEN("Method is NULL"));

	Cb fn = invokedMethod->mFunction;
	DBG_ASSERT(fn != NULL, BEER_WIDEN("Abstract method"));

	uint16 i = 0;
	int32 elemi = 0;

#ifdef BEER_STACK_DEBUGGING
#else
	Object** objPtr = NULL;
#endif // BEER_STACK_DEBUGGING

	uint16 returnCount = invokedMethod->getReturnsCount();
	uint16 paramsCount = invokedMethod->getParamsCount();

	uint32 pushedSize
		= sizeof(StackRef<Object>) * returnCount						// returns
		+ sizeof(StackRef<Object>) * paramsCount						// params
		+ sizeof(StackRef<Object>)										// receiver
		+ sizeof(Thread*);												// thread
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////// within the next block you should not do anything with the stack 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(i = 0; i < returnCount; i++)
	{
		elemi = -(static_cast<int32>(returnCount) - static_cast<int32>(i)) - static_cast<int32>(paramsCount) - 2;

#ifdef BEER_STACK_DEBUGGING
		__asm // push return
		{
			push elemi;
			push frame;
		}
#else
		objPtr = frame->stackTopPtr(elemi);
		__asm // push return
		{
			push objPtr;
		}
#endif // BEER_STACK_DEBUGGING
	}
	for(i = 0; i < paramsCount; i++)
	{
		elemi = -(static_cast<int32>(paramsCount) - static_cast<int32>(i)) - 2;

#ifdef BEER_STACK_DEBUGGING
		__asm // push param
		{
			push elemi;
			push frame;
		}
#else
		objPtr = frame->stackTopPtr(elemi);
		__asm // push return
		{
			push objPtr;
		}
#endif // BEER_STACK_DEBUGGING
	}
#ifdef BEER_STACK_DEBUGGING
	__asm
	{
		// push receiver
		push -2;
		push frame;
	}
#else
	objPtr = frame->stackTopPtr(-2);
	__asm
	{
		push objPtr;
	}
#endif // BEER_STACK_DEBUGGING
	__asm
	{
		// push Thread*
		push thread;

		// call function
		call fn;

		// fix stack
		add esp, pushedSize;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//frame->stackMoveTop(-(paramsCount + 2));  // +2: receiver, method

	thread->closeFrame();
}

void BEER_CALL Method::getName(Thread* thread, StackRef<Method> receiver, StackRef<String> ret)
{
	ret = receiver->getName();
}

void BEER_CALL Method::setName(Thread* thread, StackRef<Method> receiver, StackRef<String> value)
{
	receiver->setName(*value);
}

void BEER_CALL Method::getReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret)
{
	DBG_ASSERT(index->getData() < receiver->getReturnsCount(), BEER_WIDEN("Unknown return"));
	Object::getChild(thread, receiver, METHOD_CHILDREN_COUNT + index->getData(), ret);
}

void BEER_CALL Method::setReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value)
{
	DBG_ASSERT(index->getData() < receiver->getReturnsCount(), BEER_WIDEN("Unable to add more returns"));
	Object::setChild(thread, receiver, METHOD_CHILDREN_COUNT + index->getData(), value);
}

void BEER_CALL Method::getParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret)
{
	DBG_ASSERT(index->getData() < receiver->getParamsCount(), BEER_WIDEN("Unknown argument"));
	Object::getChild(thread, receiver, METHOD_CHILDREN_COUNT + receiver->getReturnsCount() + index->getData(), ret);
}

void BEER_CALL Method::setParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value)
{
	DBG_ASSERT(index->getData() < receiver->getParamsCount(), BEER_WIDEN("Unable to add more arguments"));
	Object::setChild(thread, receiver, METHOD_CHILDREN_COUNT + receiver->getReturnsCount() + index->getData(), value);
}

void Method::getParam(Thread* thread, StackRef<Method> receiver, StackRef<Param> ret, Integer::IntegerData index)
{
	DBG_ASSERT(index < receiver->getParamsCount(), BEER_WIDEN("Unknown argument"));
	Object::getChild(thread, receiver, METHOD_CHILDREN_COUNT + receiver->getReturnsCount() + index, ret);
}

void Method::MethodTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* inst)
{
	Class::DefaultInstanceTraverser(receiver, klass, inst);

	Method* instance = static_cast<Method*>(inst);

	receiver->traverseObjectPtr(reinterpret_cast<Object**>(&instance->mName));
	receiver->traverseObjectPtr(reinterpret_cast<Object**>(&instance->mPool));

	uint32 childrenCount = Method::METHOD_CHILDREN_COUNT + instance->getParamsCount() + instance->getReturnsCount();
	for(uint32 i = 0; i < childrenCount; i++)
	{
		receiver->traverseObjectPtr(&instance->getChildren()[i]);
	}
}