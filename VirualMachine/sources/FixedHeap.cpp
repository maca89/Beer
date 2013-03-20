#include "stdafx.h"
#include "FixedHeap.h"

#include "Integer.h"
#include "Object.h"

using namespace Beer;

FixedHeap::~FixedHeap()
{
	::free(mMemory);
}

void FixedHeap::init()
{
	mMemory = ::malloc(mSize);
}

Object* FixedHeap::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount);

	if (!canAlloc(size)) return NULL;

	Object* obj = reinterpret_cast<Object*>(reinterpret_cast<byte*>(mMemory) + mFilled + preOffset);
	memset(obj, 0, size);

	obj->setGCFlag(Object::GC_WHITE);
	obj->setTypeFlag(Object::TYPE_DIRECT_PTR);
	obj->setType(NULL);
	obj->setStaticSize(staticSize);
		
	// children array is at the end of object
	//if(childrenCount > 0)
	/*{
		if(!Integer::canBeInlineValue(childrenCount)) // TODO
		{
			throw GCException(BEER_WIDEN("Not yet implemented"));
		}

		Object** children = reinterpret_cast<Object**>(reinterpret_cast<byte*>(obj) + staticSize);
		memset(children, 0, childrenCount * sizeof(void*));
		children[0] = Integer::makeInlineValue(childrenCount); // TODO: if cannot be inlined, create full object!!!
		obj->setChildren(children);
	}*/

	mFilled += size;

	return obj;
}

void * FixedHeap::alloc(uint32 size)
{
	size = roundSize(size);

	if (!canAlloc(size)) return NULL;

	void * mem = reinterpret_cast<void *>(reinterpret_cast<size_t>(mMemory) + mFilled);

	mFilled += size;

	return mem;
}

void FixedHeap::free(Object* obj)
{
}