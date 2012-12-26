#pragma once
#include "prereq.h"
#include "GarbageCollector.h"
#include "Object.h"


namespace Beer
{
	class MarkAndSweepGC : public GarbageCollector
	{
	public:
		typedef std::list<Object*> ObjectList;

		/*enum CollectedObjectFlag
		{
			OBJECTFLAG_CLEAR = 0,
			OBJECTFLAG_INUSE = 1,
		};*/

	protected:
		ObjectList mObjects;

	public:
		MarkAndSweepGC();
		virtual ~MarkAndSweepGC();

		virtual void free(/*Reference<*/Object* /*>*/ obj);

		// Garbage Collector
		virtual /*Reference<*/Object* /*>*/ alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset = 0);
		virtual void collect();

	protected:
		void free(ObjectList::iterator& it);
	};
};