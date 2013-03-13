#include "stdafx.h"
#include "Object.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Debugger.h"

using namespace Beer;


void BEER_CALL Object::init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Object::operatorString(Thread* thread, StackRef<Object> receiver, StackRef<String> ret)
{
	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush(
		thread->getClass(receiver)
	));

	Class::getName(thread, klass, ret);
	frame->stackMoveTop(-1); // pop klass
}

void BEER_CALL Object::setClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> param)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->setChild(receiver, param, CHILD_ID_CLASS);
}

void BEER_CALL Object::getClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->getChild(receiver, ret, CHILD_ID_CLASS);
}

void BEER_CALL Object::getChild(Thread* thread, StackRef<Object> receiver, StackRef<Integer> index, StackRef<Object> ret)
{
	Object::getChild(thread, receiver, ret, index->getData());
}

void BEER_CALL Object::setChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> child, StackRef<Integer> index)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->setChild(receiver, child, index->getData());
}

void Object::getChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret, int64 index)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->getChild(receiver, ret, index);
}

void Object::setChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> child, int64 index)
{
	NULL_ASSERT(*receiver);
	thread->getGC()->setChild(receiver, child, index);
}



/*ClassReflection* ObjectClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<ObjectClass>(name, 0, 0, 0);
}*/

/*void ObjectClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	//MethodReflection* initMethod = loader->createMethod<MethodReflection>(BEER_WIDEN("Object"), 1, 0);
	//initMethod->setFunction(&BeerObject_init);
	//klass->setMethod(0, vm->createPair(vm->createString((string(klass->getName()) + BEER_WIDEN("::Object()")).c_str()), initMethod));
}*/