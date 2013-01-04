#pragma once
#include "prereq.h"
#include "GarbageCollector.h"
#include "Object.h"


namespace Beer
{
	class MarkAndSweepGC : public GarbageCollector
	{
	public:
		MarkAndSweepGC();
		virtual ~MarkAndSweepGC();

		// Garbage Collector
		virtual Object* alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset = 0);
		virtual void collect();
	};
};