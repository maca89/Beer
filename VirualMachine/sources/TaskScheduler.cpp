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
			TrampolineThread thread(mVM, mGC);
			thread.init();

			thread.setContext(task->getContext());
			thread.trampoline();

			/*if(!task->isCompleted())
			{
				mScheduled.push(task);
			}*/
		}
	}
}

void TaskScheduler::schedule(Thread* thread, StackRef<Task> task)
{
	TaskContext* context = task->getContext();
	context->init(thread->getHeap());

	Frame* frame = context->getFrame();
	frame->stackPush(*task); // push receiver
	StackRef<Method> method(frame, frame->stackPush()); // push method
				
	// fetch method, TODO: cache
	{
		StackRef<Class> klass(frame, frame->stackPush());
		thread->getType(task, klass);
		DBG_ASSERT(*klass, BEER_WIDEN("Task has no type"));

		StackRef<String> selector(frame, frame->stackPush()); // TODO
		thread->createString(selector, BEER_WIDEN("Task::run()")); // TODO: cache

		Class::findMethod(thread, klass, selector, method);

		if(method.isNull())
		{
			throw MethodNotFoundException(*task, *klass, *selector); // TODO
		}

		frame->stackPop(); // pop selector
		frame->stackPop(); // pop klass
	}

	context->openFrame();
	task->markScheduled();
	task->unmarkCompleted();
	task->unmarkCanceled();
	task->unmarkFailed();
	mScheduled.push(*task);
}

void TaskScheduler::wait(Thread* thread, StackRef<Task> who, StackRef<Task> whatFor)
{
}