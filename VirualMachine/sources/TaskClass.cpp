#include "stdafx.h"
#include "TaskClass.h"
#include "MethodReflection.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"

using namespace Beer;


/*void Task::start()
{
	mThread->run();
	//mThread->wait(); // TODO: elsewhere
}*/


/*void BEER_CALL BeerTask_init(VirtualMachine* vm, StackFrame* frame, StackRef<Task> receiver, StackRef<Task> ret)
{
	ret = receiver;
}*/

/*void BEER_CALL BeerTask_run(VirtualMachine* vm, StackFrame* frame, StackRef<Task> receiver)
{
	// nothing, just an interface
}*/

void BEER_CALL BeerTask_dorun(VirtualMachine* vm, StackFrame* frame, StackRef<Object> receiver)
{
	TrampolineThread* thread = new TrampolineThread(vm);
	vm->getThreads().insert(thread);

	thread->openStackFrame()->stackPush(receiver.get()); // push receiver
	thread->openStackFrame()->method = receiver->getClass()->findMethod(BEER_WIDEN("Main::run()")); // ugly

	thread->run();
	thread->wait(); // ugly
}

ClassReflection* TaskClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<TaskClass>(name, 1, 0, 1); // TODO: interfaces
}

void TaskClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->markAsInterface();
	klass->extends(0, vm->getObjectClass());

	MethodReflection* method = NULL;
	uint16 methodi = 0;

	/*method = loader->createMethod<MethodReflection>(BEER_WIDEN("Task"), BEER_WIDEN("Task::Task()"), 1, 0);
	method->setFunction(&BeerTask_init);
	klass->setMethod(methodi++, method);*/

	/*method = loader->createMethod<MethodReflection>(BEER_WIDEN("run"), BEER_WIDEN("Task::run()"), 0, 0);
	method->setFunction(&BeerTask_run);
	klass->setMethod(methodi++, method);*/

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("dorun"), BEER_WIDEN("Task::dorun()"), 0, 0);
	method->setFunction(&BeerTask_dorun);
	klass->setMethod(methodi++, method);
}