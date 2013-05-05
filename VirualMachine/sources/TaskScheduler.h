#pragma once
#include "prereq.h"
#include "StackRef.h"
#include "InterlockedQueue.h"
#include "Task.h"
#include "TaskContext.h"
#include "Mutex.h"


namespace Beer
{
	class Task;
	class WorkerThread;
	class VirtualMachine;
	class GenerationalGC;

	class TaskScheduler
	{
	public:
		struct WaitingTask
		{
			Task* who;
			Task* whatFor;

			INLINE WaitingTask() : who(NULL), whatFor(NULL) {}
			INLINE WaitingTask(Task* who, Task* whatFor) : who(who), whatFor(whatFor) {}
		};

		enum
		{
			SCHEDULER_TIME_FRAME = 40 // ms
		};

		typedef InterlockedQueue<Task*> TaskQueue;
		typedef InterlockedQueue<WorkerThread*> ThreadQueue;
		typedef InterlockedQueue<WaitingTask> WaitingTaskQueue;

		/*typedef void (*PushIdleFn)(CriticalSection* cs, ThreadQueue* queue, WorkerThread* thread);
		typedef WorkerThread* (*PopIdleFn)(CriticalSection* cs, ThreadQueue* queue);*/

	protected:
		VirtualMachine* mVM;
		GenerationalGC* mGC;

		volatile bool mSafePoint;
		volatile bool mRunning;

		// tasks
		TaskQueue mActive;
		// TaskQueue mLocked;

		// threads
		ThreadQueue mIdleThreads;
		
		uint16 mThreadsCount;
		HANDLE* mThreadIdleEvents;
		WorkerThread** mAllThreads;

		// synchronization
		/*CriticalSection mIdleCriticalSection;
		volatile PushIdleFn mPushIdleFunction;
		volatile PopIdleFn mPopIdleFunction;*/

	public:
		TaskScheduler();
		~TaskScheduler();

		void init(VirtualMachine* vm, GenerationalGC* mGC, uint16 threadsCount);

		void pause();
		void resume();

		volatile bool isSafePoint() const;
		void startSafePoint();
		void stopSafePoint();

		TaskQueue* getActiveQueue();
		TaskQueue* getDoneQueue();
		TaskQueue* getScheduledQueue();
		WaitingTaskQueue* getWaitingQueue();

		void updateFramesClass(Class* klass);

		// tasks
		void addTask(Task* task);
		void done(Task* task);
		void wait(Task* who, Task* whatFor);

		// threads
		void addIdle(WorkerThread* thread);
		Task* getSomeWork();

	protected:
		void safePoint();
		void contextSwitch();
		void afterSafePoint();
		void wakeUpOneThread();	
		void processDone();
		
		bool allThreadsIdle();
		
		void updateFramesPointers();
		Task* updateFramesPointers(Task* task);
		void updateFramesPointers(TaskQueue& queue);
		void updateFramesPointers(WaitingTaskQueue& queue);
		Frame* updateFramePointers(Frame* frame);

		void updateFramesClass(WaitingTaskQueue& queue, Class* klass);
		void updateFramesClass(TaskQueue& queue, Class* klass);

		/*static void PushIdleLockFree(CriticalSection* cs, ThreadQueue* queue, WorkerThread* thread);
		static void PushIdleSynchronized(CriticalSection* cs, ThreadQueue* queue, WorkerThread* thread);

		static WorkerThread* PopIdleLockFree(CriticalSection* cs, ThreadQueue* queue);
		static WorkerThread* PopIdleSynchronized(CriticalSection* cs, ThreadQueue* queue);*/
};

	
	INLINE volatile bool TaskScheduler::isSafePoint() const
	{
		return mSafePoint;
	}

	INLINE TaskScheduler::TaskQueue* TaskScheduler::getActiveQueue()
	{
		return &mActive;
	}

	/*INLINE TaskScheduler::WaitingTaskQueue* TaskScheduler::getWaitingQueue()
	{
		return &mWaiting;
	}*/

	/*INLINE TaskScheduler::TaskQueue* TaskScheduler::getDoneQueue()
	{
		return &mDone;
	}*/

	/*NLINE TaskScheduler::TaskQueue* TaskScheduler::getScheduledQueue()
	{
		return &mScheduled;
	}*/

	/*TaskScheduler::TaskQueue* TaskScheduler::getLockedQueue()
	{
		return &mLocked;
	}*/
};
