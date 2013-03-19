#include "stdafx.h"
#include "Pool.h"
#include "Thread.h"

using namespace Beer;

void Pool::getLength(Thread* thread, StackRef<Pool> pool, uint16& length)
{
	length = pool->mSize;
}

void Pool::setLength(Thread* thread, StackRef<Pool> pool, uint16 length)
{
	pool->mSize = length;
}

void Pool::getItem(Thread* thread, StackRef<Pool> receiver, uint16 index, StackRef<Object> ret)
{
	ret = receiver->mChildren[CHILD_ID_POOL_START + index]; // TODO
}

void Pool::setItem(Thread* thread, StackRef<Pool> receiver, uint16 index, StackRef<Object> item)
{
	receiver->mChildren[CHILD_ID_POOL_START + index] = *item; // TODO
}

void Pool::createSlot(Thread* thread, StackRef<Pool> receiver, uint16& ret)
{
	ret = receiver->mNext++;
}

void Pool::hasFreeSlot(Thread* thread, StackRef<Pool> receiver, bool& ret)
{
	ret = receiver->mNext < receiver->mSize;
}

void Pool::find(Thread* thread, StackRef<Pool> receiver, StackRef<Object> item, bool& ret1, uint16& ret2)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	ret1 = false;

	uint16 length = 0;
	Pool::getLength(thread, receiver, length);
	
	StackRef<Object> myItem(frame, frame->stackPush());

	for(uint16 i = 0; i < length; i++)
	{
		Pool::getItem(thread, receiver, i, myItem);

		if(*myItem == *item)
		{
			ret2 = i;
			ret1 = true;
			break;
		}
	}
	
	frame->stackMoveTop(-1); // pop item
}

void BEER_CALL Pool::clear(Thread* thread, StackRef<Pool> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	uint16 length = 0;
	Pool::getLength(thread, receiver, length);

	StackRef<Object> null(frame, frame->stackPush());

	for(uint16 i = 0; i < length; i++)
	{
		Pool::setItem(thread, receiver, i, null);
	}

	receiver->mNext = 0; // TODO

	frame->stackMoveTop(-1); // pop null
}

void BEER_CALL Pool::copyFrom(Thread* thread, StackRef<Pool> receiver, StackRef<Pool> other)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	uint16 length = 0;
	Pool::getLength(thread, receiver, length);

	uint16 otherLength = 0;
	Pool::getLength(thread, other, otherLength);
	
	uint16 min = length < otherLength ? length : otherLength;
	StackRef<Object> item(frame, frame->stackPush());

	for(uint16 i = 0; i < min; i++)
	{
		Pool::getItem(thread, other, i, item);
		Pool::setItem(thread, receiver, i, item);
	}

	frame->stackMoveTop(-1); // pop item
	receiver->mNext = other->mNext; // TOD	
}