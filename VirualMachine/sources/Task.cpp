#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"
#include "Debugger.h"
#include "FrameInspector.h"

using namespace Beer;


void BEER_CALL Task::init(Thread* thread, StackRef<Task> receiver, StackRef<Task> ret)
{
	new (&receiver->mContext) TaskContext();
	receiver->mTaskFlags = 0;

	// init task context
	{
		TaskContext* context = receiver->getContext();
		context->init(thread->getHeap(), thread->getVM()->getFrameClass());
		Frame* frame = context->getFrame();
	
		StackRef<Method> new_receiver(frame, frame->stackPush(*receiver)); // push receiver
		StackRef<Method> new_method(frame, frame->stackPush()); // push method
				
		Class* klass = thread->getType(receiver);
		new_method = klass->getMethod(Task::METHOD_SLOT_RUN);

		context->openFrame();
	}

	// init task awaiting queue
	{
		new(&(receiver->mAwaiting)) TaskQueue();
	}
	
	receiver->markScheduled();
	receiver->unmarkCompleted();
	receiver->unmarkCanceled();
	receiver->unmarkFailed();

	ret = receiver;
}

void BEER_CALL Task::schedule(Thread* thread, StackRef<Task> receiver)
{
	NULL_ASSERT(*receiver);

	thread->getVM()->getScheduler()->addTask(*receiver);
	//return;

	/*TrampolineThread* thread2 = new TrampolineThread(thread->getVM(), thread->getGC());
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
		method = klass->findVirtualMethod(*selector);
		//Class::findMethod(thread2, klass, selector, method);
		frame->stackMoveTop(-2); // pop class, selector
	}

	NULL_ASSERT(*method); // TODO

	//thread->getVM()->getDebugger()->printFrame(thread2, frame);

	thread2->openFrame();
	thread2->run();*/
}


void BEER_CALL Task::await(Thread* thread, StackRef<Task> receiver)
{
	Task* who = thread->getTask();
	Task* whatFor = *receiver;

	if(!whatFor->isCompleted() && !whatFor->isCanceled())
	{
		thread->getVM()->getScheduler()->wait(who, whatFor);
		thread->pauseExecution();
	}
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

void Task::TaskInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* inst)
{
	Class::DefaultInstanceTraverser(receiver, klass, inst);

	Task* instance = static_cast<Task*>(inst);
	TaskContext* context = instance->getContext();

	if(context->hasFrame())
	{
		FrameInspector insp;
		insp.start(context->getFrame());
		
		while(insp.hasFrame())
		{
			Frame* frame = insp.getFrame();

			if(frame->isHeapAllocated())
			{
				Object* o = frame;
				receiver->traverseObjectPtr(&o);
				//TODO: fix pointers here
			}

			insp.nextFrame();
		}
	}
}

Class* TaskInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, Task::TASK_CHILDREN_COUNT, Task::TASK_METHODS_COUNT);
}

void TaskInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->mInstanceStaticSize = sizeof(Task);
	klass->setSuperClass(thread->getVM()->getObjectClass());
	klass->setTraverser(&Task::TaskInstanceTraverser);
	klass->markAbstract();
	
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("Task"), BEER_WIDEN("Task::Task()"), &Task::init, 1, 0);
	
	// TODO: abstract run

	// TODO: Task::schedule(TaskScheduler)
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("schedule"), BEER_WIDEN("Task::schedule()"), &Task::schedule, 0, 0);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("await"), BEER_WIDEN("Task::await()"), &Task::await, 0, 0);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("getCompleted"), BEER_WIDEN("Task::getCompleted()"), &Task::getCompleted, 0, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("getCanceled"), BEER_WIDEN("Task::getCanceled()"), &Task::getCanceled, 0, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("getFailed"), BEER_WIDEN("Task::getFailed()"), &Task::getFailed, 0, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("getId"), BEER_WIDEN("Task::getId()"), &Task::getId, 0, 1);

	Method* run = loader->addVirtualMethod(thread, klass, BEER_WIDEN("run"), BEER_WIDEN("Task::run()"), &Task::abstractRun, 0, 0);
	run->markAbstract();
	RUNTIME_ASSERT(run->getSlotId() == Task::METHOD_SLOT_RUN, BEER_WIDEN("Task::run method slot is different"));
}