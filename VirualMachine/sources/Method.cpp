#include "stdafx.h"
#include "Method.h"
#include "Object.h"
#include "GenerationalGC.h"
#include "Thread.h"
#include "Frame.h"

using namespace Beer;


void Method::runFunction(Thread* thread/*, Frame* frame*/)
{
	Frame* frame = thread->getFrame();
	//BEER_STACK_CHECK();

	Cb fn = mFunction;
	if(fn == NULL)
	{
		// TODO
		throw AbstractMethodException(this);
	}

	//NULL_ASSERT(fn);

	uint16 i = 0;
	int32 elemi = 0;

#ifdef BEER_STACK_DEBUGGING
#else
	Object** objPtr = NULL;
#endif // BEER_STACK_DEBUGGING

	uint16 returnCount = getReturnsCount();
	uint16 paramsCount = getParamsCount();

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
		// push Frame*
		//push frame;

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
	Object::getChild(thread, receiver, ret, CHILD_ID_METHOD_NAME);
}

void BEER_CALL Method::setName(Thread* thread, StackRef<Method> receiver, StackRef<String> value)
{
	Object::setChild(thread, receiver, value, CHILD_ID_METHOD_NAME);
}

void BEER_CALL Method::getReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret)
{
	DBG_ASSERT(index->getData() < receiver->getReturnsCount(), BEER_WIDEN("Unknown return"));
	Object::getChild(thread, receiver, ret, CHILD_ID_METHOD_NAME + 1 + index->getData()); // +1 for name
}

void BEER_CALL Method::setReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value)
{
	DBG_ASSERT(index->getData() < receiver->getReturnsCount(), BEER_WIDEN("Unable to add more returns"));
	Object::setChild(thread, receiver, value, CHILD_ID_METHOD_NAME + 1 + index->getData()); // +1 for name
}

void BEER_CALL Method::getParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret)
{
	DBG_ASSERT(index->getData() < receiver->getParamsCount(), BEER_WIDEN("Unknown argument"));
	Object::getChild(thread, receiver, ret, CHILD_ID_METHOD_NAME + 1 + receiver->getReturnsCount() + index->getData()); // +1 for name
}

void BEER_CALL Method::setParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value)
{
	DBG_ASSERT(index->getData() < receiver->getParamsCount(), BEER_WIDEN("Unable to add more arguments"));
	Object::setChild(thread, receiver, value, CHILD_ID_METHOD_NAME + 1 + receiver->getReturnsCount() + index->getData()); // +1 for name
}

void BEER_CALL Method::getParam(Thread* thread, StackRef<Method> receiver, StackRef<Param> ret, Integer::IntegerData index)
{
	DBG_ASSERT(index < receiver->getParamsCount(), BEER_WIDEN("Unknown argument"));
	Object::getChild(thread, receiver, ret, CHILD_ID_METHOD_NAME + 1 + receiver->getReturnsCount() + index); // +1 for name
}