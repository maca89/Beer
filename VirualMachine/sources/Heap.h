#pragma once

#include "prereq.h"
#include "Object.h"
#include "GCObject.h"

namespace Beer
{
	class Heap
	{
	public:

		virtual ~Heap()
		{ }

		virtual void init() = 0;

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
			/*int32 off = ((int64)(static_cast<T*>(reinterpret_cast<Object*>(0x1000))) - (int64)(reinterpret_cast<Object*>(0x1000)));
			if(off > 0)
			{
				off = 0; // no need
			}*/

			return static_cast<T*>(alloc(staticSize, childrenCount));
		}

		virtual Object* alloc(uint32 staticSize, uint32 childrenCount) = 0;
		virtual byte* alloc(uint32 size) = 0;
		
//		virtual void free(byte* obj) = 0;

	protected:

		INLINE void initObject(Object* obj, uint32 size, uint32 staticSize)
		{
			Object::initObject(obj, size - sizeof(GCObject), staticSize);
			GCObject::init(obj, size);
		}

		// TODO: round to 4 bytes
		INLINE uint32 roundSize(uint32 size)
		{
			return size + (size & 1); // rounds size to the closest bigger even number
		}
		
		INLINE uint32 calcSize(uint32 staticSize, uint32 childrenCount)
		{
			return roundSize(staticSize + sizeof(Object*) * childrenCount + sizeof(GCObject));
		}
	};
}