#include "stdafx.h"
#include "Mutex.h"

using namespace Beer;


CriticalSection::CriticalSection()
{
	create();
}

CriticalSection::~CriticalSection()
{
	destroy();
}

void CriticalSection::create()
{
	InitializeCriticalSection(&mCriticalSection);
}

void CriticalSection::destroy()
{
	DeleteCriticalSection(&mCriticalSection);
}

void CriticalSection::enter()	
{
	EnterCriticalSection(&mCriticalSection);
}

bool CriticalSection::tryEnter()
{
	// 0 => another thread already owns the critical section
	// !0 => the critical section is successfully entered or the current thread already owns the critical section
	return TryEnterCriticalSection(&mCriticalSection) != 0;
}

void CriticalSection::leave()
{
	LeaveCriticalSection(&mCriticalSection);
}