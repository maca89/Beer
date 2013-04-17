#include "stdafx.h"
#include "TaskScheduler.h"
#include "Thread.h"
#include "Class.h"
#include "Method.h"
#include "TrampolineThread.h"
#include "VirtualMachine.h"
#include "FrameInspector.h"
#include "GenerationalGC.h"

using namespace Beer;


TaskScheduler::TaskScheduler()
	: mVM(NULL), mGC(NULL), mSafePoint(false)
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

void TaskScheduler::initializeTasks()
{
	while(!mScheduled.empty() && !isSafePoint())
	{
		Task* task = mScheduled.pop();

		TrampolineThread* thread = mAvailableThreads.pop();
		initializeTask(thread, task);
		mAvailableThreads.push(thread);

		mActive.push(task);
	}
}

void TaskScheduler::resumeAll()
{
	while(!mActive.empty() || !mScheduled.empty())
	{
		initializeTasks();

		while(!mActive.empty())
		{
			// dbg, TODO
			if(isSafePoint())
			{
				//////////////////////////////
				if(false)
				{
					if(!mActive.empty())
					{
						Task* task = mActive.pop();
						TrampolineThread* thread = mAvailableThreads.pop();
						thread->setContext(task->getContext());

						//Frame* frame = task->getContext()->getFrames();
						FrameInspector::debugPrintFrames(thread);
					
						thread->setContext(NULL); // debug
						mAvailableThreads.push(thread);
						mActive.push(task);
					}
				}
				//////////////////////////////


				mGC->threadsSuspended();
				while(isSafePoint())
				{
					Sleep(40);
				}
				//stopSafePoint();
				afterSafePoint();
				continue;
			}

			Task* task = mActive.pop();

			// TODO: run on thread
			// TODO: mActive.push(task);

			TrampolineThread* thread = mAvailableThreads.pop();
			thread->setContext(task->getContext());
			
			// TODO: parallel
			if(thread->trampoline())
			{
				task->markCompleted();
				mDone.push(task);
			}
			else
			{
				mActive.push(task);
			}

			thread->setContext(NULL); // debug
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

		method = klass->findVirtualMethod(*selector);
		//Class::findMethod(thread, klass, selector, method);

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

void TaskScheduler::afterSafePoint()
{
	updateFramesPointers();
}

void TaskScheduler::updateFramesPointers()
{
	updateFramesPointers(mActive);
	updateFramesPointers(mWaiting);
	updateFramesPointers(mDone);
	updateFramesPointers(mScheduled);
	//updateFramesPointers(mLocked);
}

void TaskScheduler::updateFramesPointers(TaskQueue& queue)
{
	if(!queue.empty()) for(TaskQueue::iterator it = queue.begin(); it != queue.end(); it++)
	{
		Task* task = *it;

		TrampolineThread* thread = mAvailableThreads.pop();
		thread->setContext(task->getContext());

		//FrameInspector::debugPrintFrames(thread);
		task->getContext()->updateMovedPointers(mGC);
		//FrameInspector::debugPrintFrames(thread);

		thread->setContext(NULL); // debug
		mAvailableThreads.push(thread);
	}
}