#include "stdafx.h"
#include "TimerClass.h"
#include "MethodReflection.h"
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


ClassReflection* TimerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<TimerClass>(name, 1, 0, 3);
}

void TimerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->extends(0, vm->getObjectClass());

	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Timer"), BEER_WIDEN("Timer::Timer()"), 1, 0);
	method->setFunction(&BeerTimer_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("start"), BEER_WIDEN("Timer::start()"), 0, 0);
	method->setFunction(&BeerTimer_start);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("stop"), BEER_WIDEN("Timer::stop()"), 1, 0);
	method->setFunction(&BeerTimer_stop);
	klass->setMethod(methodi++, method);
}