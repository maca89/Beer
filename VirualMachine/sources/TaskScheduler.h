#pragma once
#include "prereq.h"
#include "StackRef.h"
#include "InterlockedQueue.h"
#include "Task.h"
#include "TaskContext.h"


namespace Beer
{
	class Task;
	class TrampolineThread;
	class VirtualMachine;
	class GenerationalGC;

	class TaskScheduler
	{
	public:
		typedef InterlockedQueue<Task*> TaskQueue;
		typedef InterlockedQueue<TrampolineThread*> ThreadQueue;

	protected:
		TaskQueue mActive;
		TaskQueue mWaiting;
		TaskQueue mDone;
		TaskQueue mScheduled;
		// TaskQueue mLocked;

		VirtualMachine* mVM;
		GenerationalGC* mGC;
		ThreadQueue mAvailableThreads;
		volatile bool mSafePoint;

	public:
		TaskScheduler();
		~TaskScheduler();

		void init(VirtualMachine* vm, GenerationalGC* mGC);

		void contextSwitch();
		void pauseAll();
		void resumeAll();

		void schedule(Task* task);
		void wait(Task* who, Task* whatFor);
		// void wait(Thread* thread, StackRef<Task> who, StackRef<Lock> whatFor);

		volatile bool isSafePoint() const;
		void startSafePoint();
		void stopSafePoint();

		TaskQueue* getActiveQueue();
		TaskQueue* getWaitingQueue();
		TaskQueue* getDoneQueue();
		TaskQueue* getScheduledQueue();

	protected:
		void initializeTask(Thread* thread, Task* task);
		void initializeTasks();
	};

	
	INLINE volatile bool TaskScheduler::isSafePoint() const
	{
		return mSafePoint;
	}

	INLINE void TaskScheduler::startSafePoint()
	{
		mSafePoint = true;
	}
	
	INLINE void TaskScheduler::stopSafePoint()
	{
		mSafePoint = false;
	}

	INLINE TaskScheduler::TaskQueue* TaskScheduler::getActiveQueue()
	{
		return &mActive;
	}

	INLINE TaskScheduler::TaskQueue* TaskScheduler::getWaitingQueue()
	{
		return &mWaiting;
	}

	INLINE TaskScheduler::TaskQueue* TaskScheduler::getDoneQueue()
	{
		return &mDone;
	}

	INLINE TaskScheduler::TaskQueue* TaskScheduler::getScheduledQueue()
	{
		return &mScheduled;
	}

	/*TaskScheduler::TaskQueue* TaskScheduler::getLockedQueue()
	{
		return &mLocked;
	}*/
};
