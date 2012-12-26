#include "stdafx.h"
#include "MarkAndSweepGC.h"

using namespace Beer;


//#define BEER_GC_VERBOSE

//#define BEER_GC_DEBUGGING

#ifdef BEER_GC_VERBOSE
#	define GC_DEBUG(msg) std::cout << "// msGC: " << msg << std::endl;
#else
#	define GC_DEBUG(msg)
#endif


MarkAndSweepGC::MarkAndSweepGC()
{
}

MarkAndSweepGC::~MarkAndSweepGC()
{
}

/*Reference<*/Object* /*>*/ MarkAndSweepGC::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	GC_DEBUG("Trying to allocate " << staticSize << " with " << childrenCount << " children and " << preOffset << " preOffset");

	void* mem = malloc(staticSize + (childrenCount * sizeof(Object*)));
	mem = reinterpret_cast<byte*>(mem) + preOffset;

	Object* obj = reinterpret_cast<Object*>(mem);
	memset(obj, 0, staticSize + childrenCount * sizeof(Object*) - preOffset);
	obj->setGCFlag(Object::GC_WHITE);
	mObjects.push_back(obj);

	GC_DEBUG("Alloced object " << "#" << obj);

	return obj;
}

void MarkAndSweepGC::free(/*Reference<*/Object* /*>*/ obj)
{
	for(ObjectList::iterator it = mObjects.begin(); it != mObjects.end(); it++)
	{
		if(obj == *it)
		{
			free(it);
			return;
		}
	}

	throw GCException("Tried to free unknown object");
}

void MarkAndSweepGC::free(ObjectList::iterator& it)
{
	Object* obj = *it;
	it = mObjects.erase(it);
	obj->~Object(); // call destructor
	::free(obj);
}

void MarkAndSweepGC::collect()
{
	// mark
	for(ObjectList::iterator it = mObjects.begin(); it != mObjects.end(); it++)
	{
		Object* obj = *it;
		if(true)
		{
			//if(obj->getRefCounter() > 0)
			{
				obj->setGCFlag(Object::GC_GRAY);
			}
		}
	}

	// sweep
	for(ObjectList::iterator it = mObjects.begin(); it != mObjects.end(); )
	{
		Object* obj = *it;
		if(obj->getGCFlag() == Object::GC_WHITE)
		{
			free(it);
			continue; // it incremented
		}
		it++;
	}
}