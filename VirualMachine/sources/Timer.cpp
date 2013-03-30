#include "stdafx.h"
#include "Timer.h"
#include "Method.h"
#include "Float.h"
#include "VirtualMachine.h"
#include "Frame.h"
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
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(thread, receiver, propertiesCount);

	ret = thread->getHeap()->alloc<Timer>(
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + propertiesCount->getData())
	);

	frame->stackPop(); // pop propertiesCount

	Object::setType(thread, ret, receiver);
}

Class* TimerClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, 0, 4 + Object::OBJECT_METHODS_COUNT);
}

void TimerClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->addParent(thread->getVM()->getObjectClass());

	loader->addMethod(thread, klass, BEER_WIDEN("Timer"), BEER_WIDEN("Timer::Timer()"), &Timer::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("start"), BEER_WIDEN("Timer::start()"), &Timer::start, 0, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("stop"), BEER_WIDEN("Timer::stop()"), &Timer::stop, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Timer::createInstance, 1, 0);
}