#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"
#include "Debugger.h"

using namespace Beer;


void BEER_CALL Task::init(Thread* thread, StackRef<Task> receiver, StackRef<Task> ret)
{
	ret = receiver;
}

void BEER_CALL Task::schedule(Thread* thread, StackRef<Task> receiver)
{
	NULL_ASSERT(*receiver);

	TrampolineThread* thread2 = new TrampolineThread(thread->getVM(), thread->getGC());

	thread->getVM()->getThreads().insert(thread2);

	StackFrame* frame = thread2->getStackFrame();

	// push receiver
	frame->stackPush(*receiver);

	// push method
	StackRef<Method> method(frame, frame->stackPush());

	// fetch method
	{
		// push selector
		StackRef<String> selector(frame, frame->stackPush());
		thread2->createString(selector, string(BEER_WIDEN("Task::run()")));

		// push class
		StackRef<Class> klass(frame, frame->stackPush(
			thread->getClass(receiver)
		));

		NULL_ASSERT(*klass); // TODO

		// find method
		Class::findMethod(thread2, klass, selector, method);
		frame->stackMoveTop(-2); // pop class, selector
	}

	NULL_ASSERT(*method); // TODO

	//thread->getVM()->getDebugger()->printFrameStack(thread2, frame);

	thread2->openStackFrame();
	thread2->run();
}


void BEER_CALL Task::await(Thread* thread, StackRef<Task> receiver)
{

}

void BEER_CALL Task::getCompleted(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(false);
}

void BEER_CALL Task::getCanceled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(false);
}

void BEER_CALL Task::getFailed(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(false);
}

void BEER_CALL Task::getId(Thread* thread, StackRef<Task> receiver, StackRef<Integer> ret)
{
	ret = Integer::makeInlineValue(1);
}


Class* TaskInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 7);
}

void TaskInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	loader->extendClass(klass, vm->getObjectClass());
	
	loader->addMethod(klass, BEER_WIDEN("Task"), BEER_WIDEN("Task::Task()"), &Task::init, 1, 0);
	
	// TODO: abstract run

	// TODO: Task::schedule(TaskScheduler)
	loader->addMethod(klass, BEER_WIDEN("schedule"), BEER_WIDEN("Task::schedule()"), &Task::schedule, 0, 0);
	loader->addMethod(klass, BEER_WIDEN("await"), BEER_WIDEN("Task::await()"), &Task::await, 0, 0);

	loader->addMethod(klass, BEER_WIDEN("getCompleted"), BEER_WIDEN("Task::getCompleted()"), &Task::getCompleted, 0, 1);
	loader->addMethod(klass, BEER_WIDEN("getCanceled"), BEER_WIDEN("Task::getCanceled()"), &Task::getCanceled, 0, 1);
	loader->addMethod(klass, BEER_WIDEN("getFailed"), BEER_WIDEN("TaskgetFailedgetCompleted()"), &Task::getFailed, 0, 1);
	loader->addMethod(klass, BEER_WIDEN("getId"), BEER_WIDEN("Task::getId()"), &Task::getId, 0, 1);
}