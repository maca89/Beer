#include "stdafx.h"
#include "NativeThread.h"

using namespace Beer;


NativeThread::NativeThread()
 : mId(0)
{
	mHandle = CreateThread(
		NULL,
		1*1024,//512*1024*1024, // 512MB
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