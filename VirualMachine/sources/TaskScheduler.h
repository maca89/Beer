#pragma once
#include "prereq.h"
#include "StackRef.h"
#include "InterlockedQueue.h"
#include "Task.h"


namespace Beer
{
	class Task;

	class TaskScheduler
	{
	public:
		typedef InterlockedQueue<Task> TaskQueue;

	protected:
		TaskQueue mActive;
		TaskQueue mWaiting;
		TaskQueue mDone;
		// TaskQueue mLocked;

	public:
		TaskScheduler();
		~TaskScheduler();

		void contextSwitch();
		void pauseAll();
		void resumeAll();

		void schedule(Thread* thread, StackRef<Task> task);
		void wait(Thread* thread, StackRef<Task> who, StackRef<Task> whatFor);
		// void wait(Thread* thread, StackRef<Task> who, StackRef<Lock> whatFor);
	};
};