#include "stdafx.h"
#include "Pool.h"
#include "Class.h"
#include "Thread.h"
#include "VirtualMachine.h"

using namespace Beer;


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
	
	frame->stackPop(); // pop item
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

	frame->stackPop(); // pop null
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

	frame->stackPop(); // pop item
	receiver->mNext = other->mNext; // TOD	
}

void Pool::createInstance(Thread* thread, StackRef<Class> receiver, uint16 length, StackRef<Pool> ret)
{
	throw Exception(BEER_WIDEN("Not yet implemented"));
}

void Pool::PoolInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* inst)
{
	Class::DefaultInstanceTraverser(receiver, klass, inst);

	Pool* pool = static_cast<Pool*>(inst);

	for(uint16 i = 0; i < pool->mNext; i++)
	{
		receiver->traverseObjectPtr(&pool->getChildren()[CHILD_ID_POOL_START + i]);
	}
}

Class* PoolClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, Pool::POOL_CHILDREN_COUNT, Object::OBJECT_METHODS_COUNT);
}

void PoolClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->setSuperClass(thread->getVM()->getObjectClass());
	klass->setTraverser(&Pool::PoolInstanceTraverser);

	klass->markSealed();
}