#include "stdafx.h"
#include "ClassTable.h"
#include "Thread.h"
#include "Frame.h"

using namespace Beer;


uint32 ClassTable::add(Class* klass)
{
	// ...01
	if(mNext == 1)
	{
		for(; mNext < TABLE_SIZE; mNext += 4)
		{
			mTable[mNext] = klass;
		}
		mNext = 3;
		return 1;
	}

	// ...11
	else
	{
		mTable[mNext] = klass;
		mNext += 4;
		return mNext - 4;
	}
}


Class* ClassTable::fetchClass(Thread* thread, StackRef<Object> object)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush());
	Object::getClass(thread, object, klass);

	Class* result = *klass;
	frame->stackMoveTop(-1); // pop klass
	return result;
}