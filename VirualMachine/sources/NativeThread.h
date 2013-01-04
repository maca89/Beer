#pragma once
#include "prereq.h"


namespace Beer
{
	class NativeThread
	{
	protected:
		DWORD mId;
		HANDLE mHandle;

	public:
		NativeThread();
		~NativeThread();

		void run();
		void wait(int time = INFINITE); // wait until the thread finishes or time passes

	protected:
		virtual void work() = 0;
		static DWORD WINAPI staticWork(LPVOID lpThreadParameter);
	};
};