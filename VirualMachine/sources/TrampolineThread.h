#pragma once
#include "prereq.h"
#include "Thread.h"


namespace Beer
{
	class TrampolineThread : public Thread
	{
	protected:

	public:
		INLINE TrampolineThread(VirtualMachine* vm) : Thread(vm)
		{
		}

	protected:
			virtual void work();
	};
};