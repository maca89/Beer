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

	frame->stackMoveTop(-1); // pop propertiesCount

	Class::setClass(thread, ret, receiver);
}

void TimerClassInitializer::createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret)
{
	return loader->createClass<Class>(thread, name, ret, 1, 0, 4);
}

void TimerClassInitializer::initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	{
		StackRef<Class> objectClass(frame, frame->stackPush());
		thread->getObjectClass(objectClass);
		Class::addParent(thread, klass, objectClass);
		frame->stackMoveTop(-1); //  pop objectClass
	}

	loader->addMethod(thread, klass, BEER_WIDEN("Timer"), BEER_WIDEN("Timer::Timer()"), &Timer::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("start"), BEER_WIDEN("Timer::start()"), &Timer::start, 0, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("stop"), BEER_WIDEN("Timer::stop()"), &Timer::stop, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Timer::createInstance, 1, 0);
}