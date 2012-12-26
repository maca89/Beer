#pragma once
#include "prereq.h"


namespace Beer
{
	class Thread
	{
	protected:
		DWORD mId;
		HANDLE mHandle;

	public:
		Thread();
		~Thread();

		void run();
		void wait(int time = INFINITE);

	protected:
		virtual void work() = 0;
		static DWORD WINAPI staticWork(LPVOID lpThreadParameter);
	};
};