#pragma once
#include "prereq.h"
#include "Thread.h"


namespace Beer
{
	class TrampolineThread : public Thread
	{
	protected:

	public:
		INLINE TrampolineThread(VirtualMachine* vm, GC * gc) : Thread(vm, gc)
		{
		}

	protected:
			virtual void work();
	};
};