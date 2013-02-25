#pragma once
#include "prereq.h"


namespace Beer
{
	class Object;

	// TODO: rename to Heap
	// TODO: get rid of interface
	struct GarbageCollector
	{
		virtual ~GarbageCollector() {}

		template <typename T>
		INLINE T* alloc()
		{
			return alloc<T>(/*T::CHILDREN_COUNT or 0*/ Object::OBJECT_CHILDREN_COUNT);
		}

		template <typename T>
		INLINE T* alloc(uint32 childrenCount)
		{
			return alloc<T>(sizeof(T), childrenCount);
		}

		template <typename T>
		INLINE T* alloc(uint32 staticSize, uint32 childrenCount)
		{
			// TMP FIX: when vtable of T* is before CollectedObject*
			int32 off = ((int64)(static_cast<T*>(reinterpret_cast<Object*>(0x1000))) - (int64)(reinterpret_cast<Object*>(0x1000)));
			if(off > 0)
			{
				off = 0; // no need
			}

			return static_cast<T*>(alloc(staticSize, childrenCount, -off));
		}

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount = 0, int32 preOffset = 0) = 0;
		virtual void collect() = 0;
	};
};