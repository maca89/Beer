#include "stdafx.h"
#include "MarkAndSweepGC.h"

using namespace Beer;


//#define BEER_GC_VERBOSE

//#define BEER_GC_DEBUGGING

#ifdef BEER_GC_VERBOSE
#	define GC_DEBUG(msg) cout << "// msGC: " << msg << std::endl;
#else
#	define GC_DEBUG(msg)
#endif


MarkAndSweepGC::MarkAndSweepGC()
{
}

MarkAndSweepGC::~MarkAndSweepGC()
{
}

Object* MarkAndSweepGC::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	GC_DEBUG(BEER_WIDEN("Trying to allocate ") << staticSize << " with " << childrenCount << " children and " << preOffset << " preOffset");

	void* mem = malloc(staticSize + (childrenCount * sizeof(Object*)));
	mem = reinterpret_cast<byte*>(mem) + preOffset;

	Object* obj = reinterpret_cast<Object*>(mem);
	memset(obj, 0, staticSize + childrenCount * sizeof(Object*) - preOffset);
	obj->setGCFlag(Object::GC_WHITE);

	GC_DEBUG(BEER_WIDEN("Alloced object ") << "#" << obj);

	return obj;
}

void MarkAndSweepGC::collect()
{
	throw GCException(BEER_WIDEN("Not implemented"));
}