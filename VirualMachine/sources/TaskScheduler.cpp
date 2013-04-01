#include "stdafx.h"
#include "TaskScheduler.h"
#include "Thread.h"
#include "Class.h"
#include "Method.h"
#include "TrampolineThread.h"
#include "VirtualMachine.h"

using namespace Beer;


TaskScheduler::TaskScheduler()
	: mVM(NULL), mGC(NULL)
{
}

TaskScheduler::~TaskScheduler()
{
}

void TaskScheduler::init(VirtualMachine* vm, GenerationalGC* gc)
{
	mVM = vm;
	mGC = gc;

	for(int i = 0; i < 2; i++)
	{
		TrampolineThread* thread = new TrampolineThread(mVM, mGC);
		thread->init();

		mAvailableThreads.push(thread);
	}
}

void TaskScheduler::contextSwitch()
{
}

void TaskScheduler::pauseAll()
{
}

void TaskScheduler::resumeAll()
{
	while(!mScheduled.empty())
	{
		Task* task = mScheduled.pop();

		// TODO: run on thread
		// TODO: mActive.push(task);

		// temporary
		{
			TrampolineThread* thread = mAvailableThreads.pop();
			initializeTask(thread, task);

			thread->setContext(task->getContext());
			thread->trampoline();
			thread->setContext(NULL);

			/*if(!task->isCompleted())
			{
				mScheduled.push(task);
			}*/

			mAvailableThreads.push(thread);
		}
	}
}

void TaskScheduler::schedule(Task* task)
{
	mScheduled.push(task);
}

void TaskScheduler::initializeTask(Thread* thread, Task* task)
{
	TaskContext* context = task->getContext();
	context->init(thread->getHeap());

	Frame* frame = context->getFrame();
	StackRef<Method> receiver(frame, frame->stackPush(task)); // push receiver
	StackRef<Method> method(frame, frame->stackPush()); // push method
				
	// fetch method, TODO: cache
	{
		StackRef<Class> klass(frame, frame->stackPush());
		thread->getType(receiver, klass);
		DBG_ASSERT(*klass, BEER_WIDEN("Task has no type"));

		StackRef<String> selector(frame, frame->stackPush()); // TODO
		thread->createString(selector, BEER_WIDEN("Task::run()")); // TODO: cache

		Class::findMethod(thread, klass, selector, method);

		if(method.isNull())
		{
			throw MethodNotFoundException(*receiver, *klass, *selector); // TODO
		}

		frame->stackPop(); // pop selector
		frame->stackPop(); // pop klass
	}

	context->openFrame();
	task->markScheduled();
	task->unmarkCompleted();
	task->unmarkCanceled();
	task->unmarkFailed();
}

void TaskScheduler::wait(Task* who, Task* whatFor)
{
}