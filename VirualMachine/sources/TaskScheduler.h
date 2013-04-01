#pragma once
#include "prereq.h"
#include "StackRef.h"
#include "InterlockedQueue.h"
#include "Task.h"
#include "TaskContext.h"


namespace Beer
{
	class Task;
	class VirtualMachine;
	class GenerationalGC;

	class TaskScheduler
	{
	public:
		typedef InterlockedQueue<Task*> TaskQueue;

	protected:
		TaskQueue mActive;
		TaskQueue mWaiting;
		TaskQueue mDone;
		TaskQueue mScheduled;
		// TaskQueue mLocked;
		VirtualMachine* mVM;
		GenerationalGC* mGC;

	public:
		TaskScheduler();
		~TaskScheduler();

		void init(VirtualMachine* vm, GenerationalGC* mGC);

		void contextSwitch();
		void pauseAll();
		void resumeAll();

		void schedule(Thread* thread, StackRef<Task> task);
		void wait(Thread* thread, StackRef<Task> who, StackRef<Task> whatFor);
		// void wait(Thread* thread, StackRef<Task> who, StackRef<Lock> whatFor);

	protected:
	};
};