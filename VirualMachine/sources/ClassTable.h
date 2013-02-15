#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "StackFrame.h"


namespace Beer
{
	class ClassReflection;

	class ClassTable
	{
	protected:
		static const int TABLE_SIZE = 1 << (sizeof(Object::InlineValueId) * 8);
		ClassReflection* mTable[TABLE_SIZE];
		uint32 mNext;

	public:
		INLINE ClassTable() : mNext(1)
		{
			memset(mTable, 0, TABLE_SIZE * sizeof(void*));
		}

		uint32 add(ClassReflection* klass);

		INLINE ClassReflection* translate(StackRef<Object> object) const
		{
			return translate(object.get());
		}

		INLINE ClassReflection* translate(Object* object) const
		{
			ClassReflection* klass = mTable[getInlineClassId(object)];
			if(klass == NULL) return object->getClass();
			return klass;
		}

		INLINE ClassReflection* operator[] (uint32 index) const
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