#pragma once
#include "prereq.h"
#include "Class.h"
#include "Frame.h"


namespace Beer
{
	class ClassTable
	{
	protected:
		static const int TABLE_SIZE = 1 << (sizeof(Object::InlineValueId) * 8);
		Class* mTable[TABLE_SIZE];
		uint32 mNext;

	public:
		INLINE ClassTable() : mNext(1)
		{
			memset(mTable, 0, TABLE_SIZE * sizeof(void*));
		}

		uint32 add(Class* klass);

		INLINE Class* translateInline(Object* object) const
		{
			return mTable[getInlineClassId(object)];
		}

		INLINE Class* translate(Thread* thread, StackRef<Object> object) const // deprecated
		{
			Class* klass = translateInline(*object);
			if(klass == NULL) klass = fetchClass(thread, object);
			return klass;
		}

		INLINE void translate(Thread* thread, StackRef<Object> object, StackRef<Class> ret) const
		{
			ret = translateInline(*object);
			if(ret.isNull()) Object::getType(thread, object, ret);
		}

		INLINE Class* operator[] (uint32 index) const
		{
			DBG_ASSERT(index < mNext, BEER_WIDEN("Unknown class id"));
			return mTable[index];
		}

		INLINE static Object::InlineValueId getInlineClassId(Object* object)
		{
			return reinterpret_cast<Object::InlineValueId>(object) & 15;
		}

	protected:
		static Class* fetchClass(Thread* thread, StackRef<Object> object); // deprecated
	};
};