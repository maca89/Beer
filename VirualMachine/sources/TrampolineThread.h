#pragma once
#include "prereq.h"
#include "Thread.h"


namespace Beer
{
	class TrampolineThread : public Thread
	{
	protected:

	public:
		INLINE TrampolineThread(uint16 threadId, VirtualMachine* vm, GenerationalGC * gc)
			: Thread(threadId, vm, gc)
		{
			init();
		}

		bool trampoline();

	protected:
			virtual void work();
	};
};