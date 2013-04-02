#pragma once
#include "prereq.h"
#include "Thread.h"


namespace Beer
{
	class TrampolineThread : public Thread
	{
	protected:

	public:
		INLINE TrampolineThread(VirtualMachine* vm, GenerationalGC * gc) : Thread(vm, gc)
		{
			init();
		}

		bool trampoline();

	protected:
			virtual void work();
	};
};