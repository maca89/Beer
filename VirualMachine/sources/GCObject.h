#pragma once

#include "prereq.h"
#include "Object.h"

namespace Beer
{
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
		size_t mSize;
		Object* mPtrFlag; // 30 bits of forward pointer (works for pointers aligned on 4 bytes) and 2 bits for flag

	public:

		INLINE size_t getSize() const
		{
			return mSize;
		}

		INLINE void setSize(size_t size)
		{
			mSize = size;
		}

		INLINE void clearPtrFlag()
		{
			mPtrFlag = NULL;
		}

		INLINE GCFlag getFlag() const
		{
			return static_cast<GCFlag>(reinterpret_cast<ptrdiff_t>(mPtrFlag) & 0x00000003); // last 2 bits are flag
		}

		INLINE void setFlag(GCFlag flag)
		{
			mPtrFlag = reinterpret_cast<Object*>(reinterpret_cast<ptrdiff_t>(getPtr()) | flag);
		}

		INLINE bool hasFlag(GCFlag flag) const
		{
			return getFlag() == flag;
		}

		INLINE Object* getPtr() const
		{
			return reinterpret_cast<Object*>(reinterpret_cast<ptrdiff_t>(mPtrFlag) & 0xFFFFFFFC); // first 30 bits are forward pointer
		}

		INLINE void setPtr(Object* ptr)
		{
			DBG_ASSERT((reinterpret_cast<ptrdiff_t>(ptr) & 0x3) == 0, BEER_WIDEN("Pointer is not aligned to 4 bytes"));

			mPtrFlag = reinterpret_cast<Object*>(reinterpret_cast<ptrdiff_t>(ptr) | getFlag()); 
		}

		INLINE Object* forward()
		{
			Object* fwd = getPtr();

			return fwd ? fwd : getObject();
		}

		INLINE Object* getObject()
		{
			return reinterpret_cast<Object*>(reinterpret_cast<byte*>(this) + sizeof(GCObject));
		}

		INLINE static GCObject* get(Object* obj)
		{
			return reinterpret_cast<GCObject*>(reinterpret_cast<byte*>(obj) - sizeof(GCObject));
		}

		INLINE static void init(Object* obj, size_t size)
		{
			GCObject* gcObj = GCObject::get(obj);
			gcObj->clearPtrFlag();
			gcObj->setSize(size);
		}
	};
	#pragma pack(pop)

};
