#include "stdafx.h"
#include "DynamicHeap.h"

#include "Object.h"
#include "Integer.h"

using namespace Beer;

DynamicHeap::DynamicHeap(size_t initialSize)
	:	Heap(),
		mInitialSize(initialSize)
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
	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount);

	Object* obj = reinterpret_cast<Object*>(::HeapAlloc(mHandle, 0, size));

	obj->setGCFlag(Object::GC_WHITE);
	obj->setTypeFlag(Object::TYPE_REF);
	//obj->setClass(NULL);
		
	// children array is at the end of object
	//if(childrenCount > 0)
	{
		if(!Integer::canBeInlineValue(childrenCount)) // TODO
		{
			throw GCException(BEER_WIDEN("Not yet implemented"));
		}

		Object** children = reinterpret_cast<Object**>(reinterpret_cast<byte*>(obj) + staticSize);
		memset(children, 0, childrenCount * sizeof(void*));
		children[0] = Integer::makeInlineValue(childrenCount); // TODO: if cannot be inlined, create full object!!!
		obj->setChildren(children);
	}

	return obj;
}

void* DynamicHeap::alloc(uint32 size)
{
	return NULL;
}

void DynamicHeap::free(Object* obj)
{
}