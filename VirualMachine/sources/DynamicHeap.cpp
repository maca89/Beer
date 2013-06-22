#include "stdafx.h"
#include "DynamicHeap.h"

#include "Object.h"
#include "GCObject.h"

using namespace Beer;

DynamicHeap::DynamicHeap(size_t initialSize)
	:	mInitialSize(initialSize)
{
}

DynamicHeap::~DynamicHeap()
{
	::HeapDestroy(mHandle);
}

void DynamicHeap::init()
{
	mHandle = ::HeapCreate(0, mInitialSize, 0);

	if (!mHandle) throw NotEnoughMemoryException(BEER_WIDEN("Cannot create private heap"));
}

Object* DynamicHeap::alloc(uint32 staticSize, uint32 childrenCount)
{
	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount + sizeof(GCObject));

	Object* obj = reinterpret_cast<Object*>(reinterpret_cast<byte*>(::HeapAlloc(mHandle, 0, size)) + sizeof(GCObject));

	if (obj)
	{
		initObject(obj, staticSize, size);
	}
	else
	{
		throw NotEnoughMemoryException(BEER_WIDEN("Cannot allocate from private heap"));		
	}
	
	return obj;
}

byte* DynamicHeap::alloc(size_t size)
{
	byte* obj = reinterpret_cast<byte*>(::HeapAlloc(mHandle, 0, size));

	if (!obj)
	{
		throw NotEnoughMemoryException(BEER_WIDEN("Cannot allocate from private heap"));		
	}

	return obj;
}