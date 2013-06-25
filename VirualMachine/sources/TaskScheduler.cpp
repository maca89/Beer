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
#include "Debugger.h"

using namespace Beer;

//#define BEER_SCHEDULER_VERBOSE

#ifdef BEER_SCHEDULER_VERBOSE
#define SCHEDULER_DEBUG(msg) { stringstream ss; ss << "Scheduler: " << msg << "\n"; ThreadSafeOutput(cout) << ss.str(); }
#else
#define SCHEDULER_DEBUG(msg)
#endif // BEER_SCHEDULER_VERBOSE


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef BEER_DEBUG_SAFE_POINTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PrintTraverser : TraverseObjectReceiver
{
	int deep;

	PrintTraverser(int deep = 1)
		: deep(deep)
	{
	}

	virtual void traverseObjectPtr(Object** ptrToObject)
	{
		Object* object = *ptrToObject;
		cout << object << "\n";
		
		if(object == NULL || Object::isInlineValue(object))
		{
			// nothing
		}
		else if(deep > 3)
		{
			cout << "...\n";
		}
		else
		{
			object->getType()->getTraverser()(&PrintTraverser(deep + 1), object->getType(), object);
		}
	}


};

void traverse(Object* object)
{
	cout << "[Object " << object << "]\n";
	object->getType()->getTraverser()(&PrintTraverser(), object->getType(), object);
}

void traverse(TaskScheduler::TaskQueue* queue, Debugger* debugger)
{
	for (TaskScheduler::TaskQueue::iterator it = queue->begin(); it != queue->end(); it++)
	{
		cout << "[Task " << *it << "]\n";
		traverse(*it);
		cout << "\n";

		debugger->printFrameStack((*it)->getContext()->getFrame());
	}
}

void mytest(TaskScheduler* scheduler, Debugger* debugger)
{
	traverse(scheduler->getActiveQueue(), debugger);
	int a = 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // BEER_DEBUG_SAFE_POINTS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	// start all threads
	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		WorkerThread* thread = mAllThreads[i];
		thread->run();
	}

	while(mRunning)
	{
		if(isSafePoint())
		{
			safePoint();
			continue;
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
				contextSwitch();
				continue;
			}

			// we have available thread
			wakeUpOneThread();
		}

		if(mActive.empty()) // no new work
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


	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		mAllThreads[i]->contextSwitch();
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

	mIdleThreads.clear();

	// wait for all threads to be idle (some of them were already notified, some of them are idle but some of the events were consumed)
	DWORD index = 0;
	while(index < mThreadsCount)
	{
		// wait for the rest
		DWORD result = WaitForMultipleObjects(mThreadsCount - index, &mThreadIdleEvents[index], false, SCHEDULER_TIME_FRAME);

		if(result == WAIT_TIMEOUT)
		{
			// notify the rest
			for(DWORD i = index; i < mThreadsCount; i++)
			{
				mAllThreads[i]->pauseExecution(); // set the flag
				mAllThreads[i]->getDoWorkEvent()->fire(); // possibly wake up
			}

			continue;
		}

		if(result == WAIT_FAILED)
		{
			// TODO
			continue;
		}

		// + index because the array is given smaller each time
		// + 1 because it returns an index of thread (therefore 0 = first thread is idle)
		DWORD newIndex = index + (result - WAIT_OBJECT_0) + 1;

		// save idle
		for(DWORD i = index; i < newIndex; i++)
		{
			mIdleThreads.push(mAllThreads[i]); // the thread is idle now
		}

		// notify the rest
		for(DWORD i = newIndex; i < mThreadsCount; i++)
		{
			mAllThreads[i]->pauseExecution(); // set the flag
			mAllThreads[i]->getDoWorkEvent()->fire(); // possibly wake up
		}

		index = newIndex;
	}

#ifdef BEER_DEBUG_SAFE_POINTS
	mytest(this, mVM->getDebugger());
	stopSafePoint();
#else 
	// signalize GC
	mGC->threadsSuspended();

	// TODO: event from GC, no sleeplock
	while(isSafePoint())
	{
		Sleep(SCHEDULER_TIME_FRAME);
	}
#endif // BEER_DEBUG_SAFE_POINTS
	
	afterSafePoint();
}

void TaskScheduler::wakeUpOneThread()
{
	WorkerThread* thread = mIdleThreads.pop();
	if(thread)
	{
		SCHEDULER_DEBUG("wakeUpOneThread -- thread #" << thread->getThreadId() << " -- #" << thread->getDoWorkEvent()->getHandle());
		thread->getDoWorkEvent()->fire();
	}
}

bool TaskScheduler::allThreadsIdle()
{
	// ugly, TODO
	uint16 idleCount = 0;
	for(uint16 i = 0; i < mThreadsCount; i++)
	{
		WorkerThread* thread = mAllThreads[i];
		if(thread->isIdle()) idleCount++;
	}

	return idleCount == mThreadsCount;
}

void TaskScheduler::addIdle(WorkerThread* thread)
{
	mIdleThreads.push(thread);
}

void TaskScheduler::addTask(Task* task)
{
	SCHEDULER_DEBUG("Added task " << task);

	if(task->isAwaiting())
	{
		throw SchedulerException(BEER_WIDEN("Awaiting task cannot be scheduled"));
	}
		
	mActive.push(task);
}

void TaskScheduler::done(Task* task)
{
	SCHEDULER_DEBUG("Done task " << task);
	task->markCompleted();

	Task* awaiting = NULL;
	Task::TaskQueue* queue = task->getAwaitingQueue();
	while(awaiting = queue->pop())
	{
		awaiting->unmarkAwaiting();
		mActive.push(awaiting);
	}
}

Task* TaskScheduler::getSomeWork()
{
	return mActive.pop();
}

void TaskScheduler::wait(Task* who, Task* whatFor)
{
	SCHEDULER_DEBUG("Await " << who << " for " << whatFor);

	if(who->isAwaiting())
	{
		throw SchedulerException(BEER_WIDEN("Task is already awaiting"));
	}

	whatFor->getAwaitingQueue()->push(who);
	who->markAwaiting();
}

void TaskScheduler::afterSafePoint()
{
	updateFramesPointers();
	mGC->afterCollection();
}

void TaskScheduler::updateFramesPointers()
{
	updateFramesPointers(mActive);
}

Task* TaskScheduler::updateFramesPointers(Task* oldTask)
{
	SCHEDULER_DEBUG("Updating task " << oldTask << " moved to " << static_cast<Task*>(mGC->getIdentity(oldTask)));
	Task* newTask = static_cast<Task*>(mGC->getIdentity(oldTask));

	if(newTask->getContext()->hasFrame())
	{
		WorkerThread* thread = mIdleThreads.pop();
		thread->setContext(newTask);

		newTask->getContext()->updateMovedPointers(mGC);

		mIdleThreads.push(thread);
	}

	updateFramesPointers(*newTask->getAwaitingQueue());

	return newTask;
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

/*void TaskScheduler::updateFramesPointers(WaitingTaskQueue& queue)
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
}*/

void TaskScheduler::updateFramesClass(Class* klass)
{
	updateFramesClass(mActive, klass);
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



