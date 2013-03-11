#pragma once
#include "prereq.h"
#include <list>
#include <Windows.h>

namespace Beer
{
	class Object;

	typedef std::list<Object*> Objects;

	class RememberedSet
	{
	protected:

		Objects mRoots;
		CRITICAL_SECTION mCS;

	public:

		INLINE RememberedSet()
		{
			InitializeCriticalSection(&mCS);
		}

		INLINE ~RememberedSet()
		{
			DeleteCriticalSection(&mCS);
		}

		INLINE Objects * getRoots()
		{
			return &mRoots;
		}

		INLINE void clear()
		{
			mRoots.clear(); 
		}

		INLINE void add(Object* obj)
		{
			EnterCriticalSection(&mCS);
			mRoots.push_back(obj);
			LeaveCriticalSection(&mCS);
		}
	};
}