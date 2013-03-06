#include "stdafx.h"
#include "TimerClass.h"
#include "Method.h"
#include "FloatClass.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "Heap.h"

using namespace Beer;


void BEER_CALL Timer::init(Thread* thread, StackFrame* frame, StackRef<Timer> receiver, StackRef<Timer> ret)
{
	receiver->mTimer = MiliTimer();
	receiver->mTimer.start(); // TODO
	ret = receiver;
}

void BEER_CALL Timer::start(Thread* thread, StackFrame* frame, StackRef<Timer> receiver)
{
	receiver->mTimer.start();
}

void BEER_CALL Timer::stop(Thread* thread, StackFrame* frame, StackRef<Timer> receiver, StackRef<Float> ret)
{
	thread->createFloat(ret, static_cast<Float::FloatData>(receiver->mTimer.stop()));
}

void BEER_CALL TimerClass::createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<Timer> ret)
{
	ret = thread->getHeap()->alloc<Timer>(
		Object::OBJECT_CHILDREN_COUNT + receiver->getPropertiesCount()
	);
	ret->setClass(*receiver);
}

Class* TimerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<TimerClass>(name, 1, 0, 4);
}

void TimerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());

	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Timer")));
	method->setFunction(&Timer::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Timer::Timer()")), method));

	method = loader->createMethod(0, 0);
	method->setName(vm->createString(BEER_WIDEN("start")));
	method->setFunction(&Timer::start);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Timer::start()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("stop")));
	method->setFunction(&Timer::stop);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Timer::stop()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&TimerClass::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
}