#pragma once
#include "prereq.h"
#include "Class.h"
#include "StackFrame.h"


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

		INLINE Class* translate(StackRef<Object> object) const
		{
			return translate(object.get());
		}

		INLINE Class* translate(Object* object) const
		{
			Class* klass = mTable[getInlineClassId(object)];
			if(klass == NULL) return object->getClass<Class>();
			return klass;
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

		/*INLINE static bool isInlineValue(Object* object)
		{
			return (reinterpret_cast<Object::InlineValueId>(object) & 1);
		}*/

	};
};