#include "stdafx.h"
#include "NativeThread.h"

using namespace Beer;


NativeThread::NativeThread()
 : mId(0)
{
	mHandle = CreateThread(
		NULL,
		4*1024, // 4KB is default page size, however the real (kernel) stack may be up to 64KB
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
	NativeThread* t = reinterpret_cast<NativeThread*>(lpThreadParameter);
	try
	{
		t->work();
	}
	catch(Exception& ex)
	{
		//vm->getDebugger()->printCallStack();
		//vm->getDebugger()->printStack();

		//if(!mVM->getDebugger()->catchException(this, frame, ex))
		//{
		//}
		
		//mVM->getDebugger()->printFrameStack(this, oldFrame);
		cout << std::endl << "Unhandled exception on a NativeThread: " << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;
		return 1;
	}

	return 0;
}