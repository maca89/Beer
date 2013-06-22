#pragma once

#include "prereq.h"

namespace Beer
{
	class Object;

	#pragma pack(push, 1)
	class GCObject
	{
	public:
		
		enum GCFlag
		{
			GC_FLAG_WHITE = 0,
			GC_FLAG_GREY = 1,
			GC_FLAG_BLACK = 2,
		};

	protected:
		uint32 mSize;
		uint32 mData; // 30 bits of forward pointer (works for pointers aligned on 4 bytes) and 2 bits for flag or age

	public:

		INLINE uint32 getSize() const
		{
			return mSize;
		}

		INLINE void setSize(uint32 size)
		{
			mSize = size;
		}

		INLINE void clearData()
		{
			mData = 0;
		}

		INLINE GCFlag getFlag() const
		{
			return static_cast<GCFlag>(mData & 0x00000003); // last 2 bits are flag
		}

		INLINE void setFlag(GCFlag flag)
		{
			mData = reinterpret_cast<uint32>(getPtr()) | flag;
		}

		INLINE bool hasFlag(GCFlag flag) const
		{
			return getFlag() == flag;
		}

		INLINE uint32 getAge() const
		{
			return static_cast<uint32>(mData & 0x00000003); // last 2 bits are age
		}

		INLINE void setAge(uint32 age) 
		{
			DBG_ASSERT(age < 4, BEER_WIDEN("Age of object cannot be more than 3"));

			mData = reinterpret_cast<uint32>(getPtr()) | age;
		}

		INLINE bool isForwarded() const
		{
			return getPtr() != NULL;
		}

		INLINE GCObject* getPtr() const
		{
			return reinterpret_cast<GCObject*>(mData & 0xFFFFFFFC); // first 30 bits are forward pointer
		}

		INLINE void setPtr(GCObject* ptr)
		{
			DBG_ASSERT((reinterpret_cast<uint32>(ptr) & 0x3) == 0, BEER_WIDEN("Pointer is not aligned to 4 bytes"));

			mData = reinterpret_cast<uint32>(ptr) | getFlag(); 
		}

		INLINE Object* forward()
		{
			GCObject* fwd = getPtr();

			return fwd ? fwd->getObject() : getObject();
		}

		INLINE Object* getObject()
		{
			return reinterpret_cast<Object*>(reinterpret_cast<byte*>(this) + sizeof(GCObject));
		}

		INLINE static GCObject* get(Object* obj)
		{
			return reinterpret_cast<GCObject*>(reinterpret_cast<byte*>(obj) - sizeof(GCObject));
		}

		INLINE GCObject* nextObject()
		{
			return reinterpret_cast<GCObject*>(reinterpret_cast<byte*>(this) + mSize);
		}

		INLINE static void init(Object* obj, size_t size)
		{
			GCObject* gcObj = GCObject::get(obj);
			gcObj->clearData();
			gcObj->setSize(size);
		}
	};
	#pragma pack(pop)

};
