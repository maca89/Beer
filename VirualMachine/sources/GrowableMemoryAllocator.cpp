#include "stdafx.h"
#include "GrowableMemoryAllocator.h"

using namespace Beer;


PVOID GrowableMemoryAllocator::gExceptionHandlerPtr = NULL;
GrowableMemoryAllocator::length_t GrowableMemoryAllocator::gPageSize = 0;


struct GrowableMemoryAllocator::StaticInitializer
{
	INLINE StaticInitializer()
	{
		gExceptionHandlerPtr = ::AddVectoredExceptionHandler(true, ExceptionHandler);
		
		SYSTEM_INFO si;
		::GetSystemInfo(&si);
		gPageSize = si.dwPageSize;
	}

	INLINE ~StaticInitializer()
	{
		if(gExceptionHandlerPtr)
		{
			::RemoveVectoredExceptionHandler(gExceptionHandlerPtr);
			gExceptionHandlerPtr = NULL;
		}
	}
};

GrowableMemoryAllocator::StaticInitializer GrowableMemoryAllocator::gStaticInitializer;