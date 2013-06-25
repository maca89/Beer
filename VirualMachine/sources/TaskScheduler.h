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

	protected:
		VirtualMachine* mVM;
		GenerationalGC* mGC;

		volatile bool mSafePoint;
		volatile bool mRunning;

		// tasks
		TaskQueue mActive;
		// TaskQueue mLocked; // TODO

		// threads
		ThreadQueue mIdleThreads;
		
		uint16 mThreadsCount;
		HANDLE* mThreadIdleEvents;
		WorkerThread** mAllThreads;

	public:
		TaskScheduler();
		~TaskScheduler();

		void init(VirtualMachine* vm, GenerationalGC* mGC, uint16 threadsCount);

		void pause();
		void resume();

		volatile bool isSafePoint() const;
		void startSafePoint();
		void stopSafePoint();

		void updateFramesClass(Class* klass);

		// tasks
		void addTask(Task* task);
		void done(Task* task);
		void wait(Task* who, Task* whatFor);
		TaskQueue* getActiveQueue();

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
		//void updateFramesPointers(WaitingTaskQueue& queue);
		Frame* updateFramePointers(Frame* frame);

		void updateFramesClass(WaitingTaskQueue& queue, Class* klass);
		void updateFramesClass(TaskQueue& queue, Class* klass);
};

	
	INLINE volatile bool TaskScheduler::isSafePoint() const
	{
		return mSafePoint;
	}

	INLINE TaskScheduler::TaskQueue* TaskScheduler::getActiveQueue()
	{
		return &mActive;
	}
};
