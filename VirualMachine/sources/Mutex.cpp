#include "stdafx.h"
#include "Mutex.h"
#include "ThreadSafeOutput.h"

using namespace Beer;

//#define BEER_CRITICAL_SECTION_VERBOSE

#ifdef BEER_CRITICAL_SECTION_VERBOSE
#define CRITICAL_SECTION_DEBUG(msg) { stringstream ss; ss << "CriticalSection#" << this << ": " << msg << "\n"; ThreadSafeOutput(cout) << ss.str(); }
#else
#define CRITICAL_SECTION_DEBUG(msg)
#endif // BEER_SCHEDULER_VERBOSE


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
	CRITICAL_SECTION_DEBUG("Enter");
}

bool CriticalSection::tryEnter()
{
	// 0 => another thread already owns the critical section
	// !0 => the critical section is successfully entered or the current thread already owns the critical section
	bool succ = TryEnterCriticalSection(&mCriticalSection) != 0;
	CRITICAL_SECTION_DEBUG("TryEnter:" << succ);
	return succ;
}

void CriticalSection::leave()
{
	LeaveCriticalSection(&mCriticalSection);
	CRITICAL_SECTION_DEBUG("Leave");
}