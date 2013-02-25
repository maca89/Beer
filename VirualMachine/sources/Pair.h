#pragma once
#include "prereq.h"
#include "Object.h"

namespace Beer
{
	class VirtualMachine;

	class Pair : public Object
	{
	public:
		template <typename T>
		INLINE T* getFirst()
		{
			return getChild<T>(OBJECT_CHILDREN_COUNT);
		}

		INLINE void setFirst(Object* object)
		{
			setChild(OBJECT_CHILDREN_COUNT, object);
		}

		template <typename T>
		INLINE T* getSecond()
		{
			return getChild<T>(OBJECT_CHILDREN_COUNT + 1);
		}

		INLINE void setSecond(Object* object)
		{
			setChild(OBJECT_CHILDREN_COUNT + 1, object);
		}
	};
};