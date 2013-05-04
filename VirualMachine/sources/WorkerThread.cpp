#include "stdafx.h"
#include "WorkerThread.h"
#include "VirtualMachine.h"
#include "ThreadSafeOutput.h"

using namespace Beer;

//#define BEER_SCHEDULER_VERBOSE

#ifdef BEER_SCHEDULER_VERBOSE
#define WORKER_DEBUG(msg) { stringstream ss; ss << "Worker#" << mThreadId << ": " << msg << "\n"; ThreadSafeOutput(cout) << ss.str(); }
#else
#define WORKER_DEBUG(msg)
#endif // BEER_SCHEDULER_VERBOSE

WorkerThread::WorkerThread(uint16 threadId, VirtualMachine* vm, GenerationalGC* gc)
	: TrampolineThread(threadId, vm, gc), mContextSwitch(false), mWorking(false),
		mIdleEvent(SynchronizationEvent::EVENT_AUTO_RESET), // EVENT_MANUAL_RESET
		mDoWorkEvent(SynchronizationEvent::EVENT_AUTO_RESET)
{

}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::contextSwitch()
{
	WORKER_DEBUG("contextSwitch");
	mExecutionPaused = true;
	mContextSwitch = true;
}

void WorkerThread::idle()
{
	WORKER_DEBUG("idle");
	mVM->getScheduler()->addIdle(this);
	mIdleEvent.fire();

	mDoWorkEvent.reset();
	mDoWorkEvent.wait();
	WORKER_DEBUG("do work");
}

void WorkerThread::work(Task* task)
{
	WORKER_DEBUG("work -- task");

	TaskScheduler* scheduler = getVM()->getScheduler();

	setContext(task);

	if(trampoline()) // TODO: context switch
	{
		scheduler->done(task);
	}
	else
	{
		scheduler->addTask(task);
	}
}

void WorkerThread::terminate()
{
	mWorking = false;
}

void WorkerThread::work()
{
	mWorking = true;
	mExecutionPaused = false;
	TaskScheduler* scheduler = getVM()->getScheduler();

	mDoWorkEvent.wait();

	while(mWorking)
	{
		//WORKER_DEBUG("work -- iteration");

		if(mContextSwitch)
		{
			mContextSwitch = false;
		}
		
		if (mExecutionPaused)
		{
			mExecutionPaused = false;
			idle();
		}

		//WORKER_DEBUG("work -- get some work");
		Task* task = scheduler->getSomeWork();

		if(task)
		{
			work(task);
		}
		else
		{
			idle();
		}
	}
}