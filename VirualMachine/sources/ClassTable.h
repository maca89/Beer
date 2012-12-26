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
		ClassTable() : mNext(1)
		{
			memset(mTable, 0, TABLE_SIZE * sizeof(void*));
		}

		uint32 add(ClassReflection* klass);

		INLINE ClassReflection* translate(StackRef<Object> object)
		{
			return translate(object.get());
		}

		INLINE ClassReflection* translate(Object* object)
		{
			ClassReflection* klass = mTable[reinterpret_cast<Object::InlineValueId>(object)];
			if(klass == NULL) return object->getClass();
			return klass;
		}

		/*INLINE bool isInlineValue(Object* object) const
		{
			return (reinterpret_cast<Object::InlineValueId>(object) & 1);
		}*/

	};
};