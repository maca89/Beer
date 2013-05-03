#include "stdafx.h"
#include "TaskScheduler.h"
#include "Thread.h"
#include "Class.h"
#include "Method.h"
#include "WorkerThread.h"
#include "VirtualMachine.h"
#include "FrameInspector.h"
#include "GenerationalGC.h"
#include "ThreadSafeOutput.h"

using namespace Beer;

//#define BEER_SCHEDULER_VERBOSE

#ifdef BEER_SCHEDULER_VERBOSE
#define SCHEDULER_DEBUG(msg) { stringstream ss; ss << "Scheduler: " << msg << "\n"; ThreadSafeOutput(cout) << ss.str(); }
#else
#define SCHEDULER_DEBUG(msg)
#endif // BEER_SCHEDULER_VERBOSE

TaskScheduler::TaskScheduler()
	: mVM(NULL), mGC(NULL), mSafePoint(false), mThreadIdleEvents(NULL), mRunning(false), mThreadsCount(0)
{
	
}

TaskScheduler::~TaskScheduler()
{
}

void TaskScheduler::init(VirtualMachine* vm, GenerationalGC* gc, uint16 threadsCount)
{
	mVM = vm;
	mGC = gc;
	mThreadsCount = threadsCount;
	mThreadIdleEvents = new HANDLE[threadsCount];
	mAllThreads = new WorkerThread* [threadsCount];

	for(uint16 i = 0; i < threadsCount; i++)
	{
		WorkerThread* thread = new WorkerThread(i, mVM, mGC);
		thread->init();

		mThreadIdleEvents[i] = thread->getIdleEvent()->getHandle();
		mAllThreads[i] = thread;

		mIdleThreads.push(thread);
	}
}

void TaskScheduler::pause()
{
	mRunning = false;
	// TODO: event
}

void TaskScheduler::resume()
{
	mRunning = true;
	mIdleThreads.clear();

	// start all threads
	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		WorkerThread* thread = mAllThreads[i];
		thread->run();
		mIdleThreads.push(thread);
	}

	while(mRunning)
	{
#ifdef BEER_SCHEDULER_VERBOSE
		{
			stringstream ss;
			ss << "Scheduler:[";

			ThreadQueue q;
			WorkerThread* t = NULL;

			// print idle
			ss << "[Idle:";
			while(t = mIdleThreads.pop())
			{
				ss << t->getThreadId() << " ";
				q.push(t);
			}
			ss << "]";

			// fix idle
			while(t = q.pop())
			{
				mIdleThreads.push(t);
			}

			ss << "]\n";
			ThreadSafeOutput(cout) << ss.str();
		}
#endif // BEER_SCHEDULER_VERBOSE

		if(isSafePoint())
		{
			safePoint();
			continue;
		}

		if(!mDone.empty())
		{
			mDone.clear(); // dbg
		}

		if(!mActive.empty()) // we have some tasks
		{
			if(mIdleThreads.empty()) // no threads available
			{
				//SCHEDULER_DEBUG("resume -- no worker -- wait");
				// wait a while until maybe some appear
				WaitForMultipleObjects(mThreadsCount, mThreadIdleEvents, false, SCHEDULER_TIME_FRAME);
			}

			if(mIdleThreads.empty()) // still no threads available
			{
				//SCHEDULER_DEBUG("resume -- no worker -- context switch");
				//contextSwitch();
				continue;
			}

			// we have available thread
			wakeUpOneThread();
		}

		if(mActive.empty() && mWaiting.empty() && mDone.empty()) // no new work
		{
			// is anybody working?
			if(allThreadsIdle())
			{
				SCHEDULER_DEBUG("resume -- all done");
				mRunning = false;
				continue;
			}
			// otherwise somebody is still working, we have to just wait
		}

		if(mActive.empty()) // no work, lets just wait a while
		{
			//SCHEDULER_DEBUG("resume -- no work");
			WaitForMultipleObjects(mThreadsCount, mThreadIdleEvents, false, SCHEDULER_TIME_FRAME);
		}
	}

	// start all threads
	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		WorkerThread* thread = mAllThreads[i];
		thread->terminate();
	}
}

void TaskScheduler::contextSwitch()
{
	SCHEDULER_DEBUG("contextSwitch");

	// TODO

	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		mAllThreads[i]->contextSwitch();
		//mAllThreads[i]->getDoWorkEvent()->fire(); // dbg, TODO
	}
}

void TaskScheduler::startSafePoint()
{
	SCHEDULER_DEBUG("startSafePoint");

	mSafePoint = true;

	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		mAllThreads[i]->pauseExecution();
	}
}
	
void TaskScheduler::stopSafePoint()
{
	SCHEDULER_DEBUG("stopSafePoint");
	mSafePoint = false;
}

void TaskScheduler::safePoint()
{
	SCHEDULER_DEBUG("safePoint");

	// wait for all threads to be idle (they **WERE** already notified)
	WaitForMultipleObjects(mThreadsCount, mThreadIdleEvents, true, INFINITE);

	// all threads are idle now
	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		mIdleThreads.push(mAllThreads[i]);
	}
	
	// signalize GC
	mGC->threadsSuspended(); 

	// TODO: event from GC, no sleeplock
	while(isSafePoint())
	{
		Sleep(40);
	}

	//stopSafePoint();
	afterSafePoint();
}

void TaskScheduler::wakeUpOneThread()
{
	WorkerThread* thread = mIdleThreads.pop();
	SCHEDULER_DEBUG("wakeUpOneThread -- thread #" << thread->getThreadId() << " -- #" << thread->getDoWorkEvent()->getHandle());
	thread->getDoWorkEvent()->fire();
}

bool TaskScheduler::allThreadsIdle()
{
	// very ugly, TODO

	ThreadQueue q;
	WorkerThread* t = NULL;
	uint16 idleCount = 0;
	while(t = mIdleThreads.pop())
	{
		q.push(t);
		idleCount++;
	}

	while(t = q.pop())
	{
		mIdleThreads.push(t);
	}

	return idleCount == mThreadsCount;
}

void TaskScheduler::addIdle(WorkerThread* thread)
{
	mIdleThreads.push(thread);
}

void TaskScheduler::addTask(Task* task)
{
	mActive.push(task);
}

void TaskScheduler::done(Task* task)
{
	mDone.push(task); // TODO: find waiting tasks
	task->markCompleted();
}

Task* TaskScheduler::getSomeWork()
{
	return mActive.pop();
}

void TaskScheduler::wait(Task* who, Task* whatFor)
{
	WaitingTask waiting(who, whatFor);
	mWaiting.push(waiting);
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
	//updateFramesPointers(mScheduled);
	//updateFramesPointers(mLocked);
}

Task* TaskScheduler::updateFramesPointers(Task* task)
{
	task = static_cast<Task*>(mGC->getIdentity(task));

	if(task->getContext()->hasFrame())
	{
		WorkerThread* thread = mIdleThreads.pop();
		thread->setContext(task);

		task->getContext()->updateMovedPointers(mGC);

		mIdleThreads.push(thread);
	}

	return task;
}

void TaskScheduler::updateFramesPointers(TaskQueue& queue)
{
	TaskQueue newQueue;

	while(!queue.empty())
	{
		newQueue.push(updateFramesPointers(queue.pop()));
	}

	while(!newQueue.empty())
	{
		queue.push(newQueue.pop());
	}
}

void TaskScheduler::updateFramesPointers(WaitingTaskQueue& queue)
{
	WaitingTaskQueue newQueue;

	while(!queue.empty())
	{
		WaitingTask waiting = queue.pop();
		waiting.who = updateFramesPointers(waiting.who);
		waiting.whatFor = updateFramesPointers(waiting.whatFor);
		newQueue.push(waiting);
	}

	while(!newQueue.empty())
	{
		queue.push(newQueue.pop());
	}
}

void TaskScheduler::updateFramesClass(Class* klass)
{
	updateFramesClass(mActive, klass);
	updateFramesClass(mWaiting, klass);
	updateFramesClass(mDone, klass);
	//updateFramesClass(mScheduled, klass);
	//updateFramesClass(mLocked, klass);
}

void TaskScheduler::updateFramesClass(WaitingTaskQueue& queue, Class* klass)
{
	for(WaitingTaskQueue::iterator it = queue.begin(); it != queue.end(); it++)
	{
		WaitingTask waiting = *it;
		waiting.who->getContext()->updateFramesClass(klass);
		waiting.whatFor->getContext()->updateFramesClass(klass);
	}
}

void TaskScheduler::updateFramesClass(TaskQueue& queue, Class* klass)
{
	for(TaskQueue::iterator it = queue.begin(); it != queue.end(); it++)
	{
		Task* task = *it;
		task->getContext()->updateFramesClass(klass);
	}
}



