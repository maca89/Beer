#pragma once
#include "prereq.h"
#include "Thread.h"


namespace Beer
{
	class WorkerThread : public Thread
	{
	public:

	protected:
		volatile bool mContextSwitch;
		volatile bool mWorking;

		SynchronizationEvent mIdleEvent;
		SynchronizationEvent mDoWorkEvent;

	public:
		WorkerThread(uint16 threadId, VirtualMachine* vm, GenerationalGC* gc);
		~WorkerThread();

		SynchronizationEvent* getIdleEvent();
		SynchronizationEvent* getDoWorkEvent();

		void contextSwitch();
		void terminate(); // you must call run to start a thread again

	protected:
			virtual void work();

			void idle();
			void work(Task* task);
	};

	// inline definitions

	INLINE SynchronizationEvent* WorkerThread::getIdleEvent()
	{
		return &mIdleEvent;
	}

	INLINE SynchronizationEvent* WorkerThread::getDoWorkEvent()
	{
		return &mDoWorkEvent;
	}
};