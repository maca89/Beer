#include "stdafx.h"
#include "TimerClass.h"
#include "Method.h"
#include "FloatClass.h"
#include "VirtualMachine.h"
#include "StackFrame.h"

using namespace Beer;


void BEER_CALL BeerTimer_init(VirtualMachine* vm, StackFrame* frame, StackRef<Timer> receiver, StackRef<Timer> ret)
{
	ret = receiver;
}

void BEER_CALL BeerTimer_start(VirtualMachine* vm, StackFrame* frame, StackRef<Timer> receiver)
{
	receiver->start();
}

void BEER_CALL BeerTimer_stop(VirtualMachine* vm, StackFrame* frame, StackRef<Timer> receiver, StackRef<Float> ret)
{
	ret = vm->createFloat(receiver->stop());
}


Class* TimerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<TimerClass>(name, 1, 0, 3);
}

void TimerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());

	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Timer")));
	method->setFunction(&BeerTimer_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Timer::Timer()")), method));

	method = loader->createMethod(0, 0);
	method->setName(vm->createString(BEER_WIDEN("start")));
	method->setFunction(&BeerTimer_start);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Timer::start()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("stop")));
	method->setFunction(&BeerTimer_stop);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Timer::stop()")), method));
}