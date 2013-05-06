#include "stdafx.h"
#include "Object.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Debugger.h"

using namespace Beer;

/*uint32 Object::getStaticSize()
{
	if(getType() == NULL)
	{
		int a = 0;
	}

	if(mStaticSize != getType()->getInstanceStaticSize())
	{
		uint32 instanceStaticSize = getType()->getInstanceStaticSize();
		int a = 0;
	}
	return mStaticSize;
}*/

void BEER_CALL Object::init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Object::operatorString(Thread* thread, StackRef<Object> receiver, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush(
		thread->getType(receiver)
	));

	Class::getName(thread, klass, ret);
	frame->stackPop(); // pop klass
}

void BEER_CALL Object::setType(Thread* thread, StackRef<Object> receiver, StackRef<Class> param)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->getIdentity(receiver)->mType = *param;
}

void BEER_CALL Object::getType(Thread* thread, StackRef<Object> receiver, StackRef<Class> ret)
{
	NULL_ASSERT(*receiver);
	ret = thread->getGC()->getIdentity(receiver)->mType;
}

/*void BEER_CALL Object::getChild(Thread* thread, StackRef<Object> receiver, StackRef<Integer> index, StackRef<Object> ret)
{
	Object::getChild(thread, receiver, ret, index->getData());
}

void BEER_CALL Object::setChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> child, StackRef<Integer> index)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->setChild(receiver, child, index->getData());
}*/

void Object::getChild(Thread* thread, StackRef<Object> receiver, int64 index, StackRef<Object> ret)
{
	NULL_ASSERT(*receiver);

#ifdef BEER_BARRIER_READ_ON
	thread->getGC()->getChild(receiver, ret, index);
#else
	ret = receiver->getChild(index);
#endif // BEER_BARRIER_READ_ON
}

void Object::setChild(Thread* thread, StackRef<Object> receiver, int64 index, StackRef<Object> child)
{
	NULL_ASSERT(*receiver);

#ifdef BEER_BARRIER_WRITE_ON
	thread->getGC()->setChild(receiver, child, index);
#else
	receiver->setChild(index, *child);
#endif // BEER_BARRIER_READ_ON
}