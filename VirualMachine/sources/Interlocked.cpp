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