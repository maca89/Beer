#include "stdafx.h"
#include "Thread.h"

using namespace Beer;


Thread::Thread()
 : mId(0)
{
	mHandle = CreateThread(
		NULL,
		0,
		&staticWork,
		this,
		CREATE_SUSPENDED,
		&mId
	);
}

Thread::~Thread()
{
}

void Thread::run()
{
	ResumeThread(mHandle);
}

void Thread::wait(int time)
{
	WaitForSingleObject(mHandle, time);
}

DWORD WINAPI Thread::staticWork(LPVOID lpThreadParameter)
{
	reinterpret_cast<Thread*>(lpThreadParameter)->work();
	return 0;
}