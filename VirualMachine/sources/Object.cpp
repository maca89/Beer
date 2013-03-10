#include "stdafx.h"
#include "Object.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL Object::init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Object::setClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> param)
{
	//NULL_ASSERT(*receiver);
	// TODO: no pop in GC, no push there

	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();
	
	frame->stackPush(*receiver); // push object
	frame->stackPush(*param); // push class

	thread->getGC()->setChild(frame, CHILD_ID_CLASS); // pops class & object
}

void BEER_CALL Object::getClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret)
{
	// TODO: pass as StackRef<Object>

	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();
	
	frame->stackPush(*receiver); // push object
	thread->getGC()->pushChild(frame, CHILD_ID_CLASS); // pops object, pushes class

	ret = frame->stackTop(); // store class
	frame->stackPop(); // pops class
}

void BEER_CALL Object::getChild(Thread* thread, StackRef<Object> receiver, StackRef<Integer> index, StackRef<Object> ret)
{
	Object::getChild(thread, receiver, ret, index->getData());
}

void BEER_CALL Object::setChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> child, StackRef<Integer> index)
{
	// TODO: no pop in GC, no push there

	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();
	
	frame->stackPush(*receiver); // push object
	frame->stackPush(*child); // push child

	thread->getGC()->setChild(frame, index->getData()); // pops child & object
}

void Object::getChild(Thread* thread, StackRef<Object> object, StackRef<Object> ret, int64 index)
{
	// TODO: pass as StackRef<Object>

	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();
	
	frame->stackPush(*object); // push object
	thread->getGC()->pushChild(frame, index); // pops object, pushes child

	ret = frame->stackTop(); // store child
	frame->stackPop(); // pops child
}

void Object::setChild(Thread* thread, StackRef<Object> object, StackRef<Object> child, int64 index)
{
	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();

	frame->stackPush(*object); // push object
	frame->stackPush(*child); // push child

	thread->getGC()->setChild(frame, index); // pops object & child
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