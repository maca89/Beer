#include "stdafx.h"
#include "Timer.h"
#include "Method.h"
#include "Float.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "Heap.h"

using namespace Beer;


void BEER_CALL Timer::init(Thread* thread, StackRef<Timer> receiver, StackRef<Timer> ret)
{
	receiver->mTimer = MiliTimer();
	receiver->mTimer.start(); // TODO
	ret = receiver;
}

void BEER_CALL Timer::start(Thread* thread, StackRef<Timer> receiver)
{
	receiver->mTimer.start();
}

void BEER_CALL Timer::stop(Thread* thread, StackRef<Timer> receiver, StackRef<Float> ret)
{
	thread->createFloat(ret, static_cast<Float::FloatData>(receiver->mTimer.stop()));
}

void BEER_CALL Timer::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Timer> ret)
{
	ret = thread->getHeap()->alloc<Timer>(
		Object::OBJECT_CHILDREN_COUNT + receiver->getPropertiesCount()
	);
	ret->setClass(*receiver);
}

Class* TimerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 4);
}

void TimerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	loader->extendClass(klass, vm->getObjectClass());

	loader->addMethod(klass, BEER_WIDEN("Timer"), BEER_WIDEN("Timer::Timer()"), &Timer::init, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("start"), BEER_WIDEN("Timer::start()"), &Timer::start, 0, 0);
	loader->addMethod(klass, BEER_WIDEN("stop"), BEER_WIDEN("Timer::stop()"), &Timer::stop, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Timer::createInstance, 1, 0);
}