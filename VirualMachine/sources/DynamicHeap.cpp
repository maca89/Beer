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
}

Object* DynamicHeap::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount/* + sizeof(GCObject)*/);

	Object* obj = reinterpret_cast<Object*>(reinterpret_cast<byte*>(::HeapAlloc(mHandle, 0, size))/* + sizeof(GCObject)*/);

	if (obj)
	{
		initObject(obj, staticSize, size);
	}
	
	return obj;
}

byte* DynamicHeap::alloc(size_t size)
{
	byte* obj = reinterpret_cast<byte*>(::HeapAlloc(mHandle, 0, size));

	return obj;
}