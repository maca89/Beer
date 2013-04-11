#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"
#include "Debugger.h"

using namespace Beer;


void BEER_CALL Task::init(Thread* thread, StackRef<Task> receiver, StackRef<Task> ret)
{
	receiver->mContext = TaskContext();
	receiver->mTaskFlags = 0;
	ret = receiver;
}

void BEER_CALL Task::schedule(Thread* thread, StackRef<Task> receiver)
{
	NULL_ASSERT(*receiver);

	TrampolineThread* thread2 = new TrampolineThread(thread->getVM(), thread->getGC());
	thread->getVM()->getThreads().insert(thread2);
	Frame* frame = thread2->getFrame();

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
			thread->getType(receiver)
		));

		NULL_ASSERT(*klass); // TODO

		// find method
		Class::findMethod(thread2, klass, selector, method);
		frame->stackMoveTop(-2); // pop class, selector
	}

	NULL_ASSERT(*method); // TODO

	//thread->getVM()->getDebugger()->printFrame(thread2, frame);

	thread2->openFrame();
	thread2->run();
}


void BEER_CALL Task::await(Thread* thread, StackRef<Task> receiver)
{

}

void BEER_CALL Task::getScheduled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->isScheduled());
}

void BEER_CALL Task::getCompleted(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->isCompleted());
}

void BEER_CALL Task::getCanceled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->isCanceled());
}

void BEER_CALL Task::getFailed(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->isFailed());
}

void BEER_CALL Task::getId(Thread* thread, StackRef<Task> receiver, StackRef<Integer> ret)
{
	ret = Integer::makeInlineValue(1);
}

void BEER_CALL Task::abstractRun(Thread* thread, StackRef<Task> receiver)
{
	Frame* frame = thread->getFrame();
	StackRef<Method> method(frame, Frame::INDEX_METHOD);
	throw AbstractMethodException(*method);
}

Class* TaskInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, Task::TASK_CHILDREN_COUNT, Task::TASK_METHODS_COUNT);
}

void TaskInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->mInstanceStaticSize = sizeof(Task);
	klass->addParent(thread->getVM()->getObjectClass());
	
	loader->addMethod(thread, klass, BEER_WIDEN("Task"), BEER_WIDEN("Task::Task()"), &Task::init, 1, 0);
	
	// TODO: abstract run

	// TODO: Task::schedule(TaskScheduler)
	loader->addMethod(thread, klass, BEER_WIDEN("schedule"), BEER_WIDEN("Task::schedule()"), &Task::schedule, 0, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("await"), BEER_WIDEN("Task::await()"), &Task::await, 0, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("getCompleted"), BEER_WIDEN("Task::getCompleted()"), &Task::getCompleted, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("getCanceled"), BEER_WIDEN("Task::getCanceled()"), &Task::getCanceled, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("getFailed"), BEER_WIDEN("Task::getFailed()"), &Task::getFailed, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("getId"), BEER_WIDEN("Task::getId()"), &Task::getId, 0, 1);

	loader->addMethod(thread, klass, BEER_WIDEN("run"), BEER_WIDEN("Task::run()"), &Task::abstractRun, 0, 0)
		->markAbstract();
}