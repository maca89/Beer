#include "stdafx.h"
#include "Interlocked.h"

using namespace Beer;


bool Interlocked::CompareAndSwap32(int32* dest, int32 oldValue, int32 newValue)
{
	_asm
	{
		mov eax, oldValue
		mov ecx, dest
		mov edx, newValue
		lock cmpxchg [ecx],edx
		mov eax,0
		setz al
	}
}

bool Interlocked::FetchAndIncrement32(int32* dest)
{
	// TODO: better

	while(true)
	{
		int32 oldValue = *dest;
		int32 newValue = oldValue + 1;

		if(CompareAndSwap32(dest, oldValue, newValue))
		{
			return true;
		}
	}
}