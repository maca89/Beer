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

		void trampoline();

	protected:
			virtual void work();
	};
};