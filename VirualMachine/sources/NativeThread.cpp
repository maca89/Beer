#include "stdafx.h"
#include "NativeThread.h"

using namespace Beer;


NativeThread::NativeThread()
 : mId(0)
{
	mHandle = CreateThread(
		NULL,
		4*1024, // 4KB is default page size, smaller value is useless
		&staticWork,
		this,
		CREATE_SUSPENDED,
		&mId
	);
}

NativeThread::~NativeThread()
{
}

void NativeThread::run()
{
	ResumeThread(mHandle);
}

void NativeThread::wait(int time)
{
	WaitForSingleObject(mHandle, time);
}

DWORD WINAPI NativeThread::staticWork(LPVOID lpThreadParameter)
{
	reinterpret_cast<NativeThread*>(lpThreadParameter)->work();
	return 0;
}