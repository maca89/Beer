#include "stdafx.h"
#include "SynchronizationEvent.h"

using namespace Beer;


SynchronizationEvent::SynchronizationEvent() : mHandle(NULL)
{
	mHandle = ::CreateEvent(NULL, false, false, NULL);
	
	if (!mHandle)
	{
		throw Exception(BEER_WIDEN("Could not create IdleEvent for thread"));
	}
}

SynchronizationEvent::~SynchronizationEvent()
{
	::CloseHandle(mHandle);
}

void SynchronizationEvent::reset()
{
	::ResetEvent(mHandle);
}

void SynchronizationEvent::fire()
{
	::SetEvent(mHandle);
}

bool SynchronizationEvent::wait(uint32 maxTime)
{
	DWORD res = ::WaitForSingleObject(mHandle, maxTime);

	if (res == WAIT_OBJECT_0)
	{
		return true;
	}

	return false;
}