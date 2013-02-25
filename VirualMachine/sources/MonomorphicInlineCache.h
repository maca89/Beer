#pragma once
#include "prereq.h"
#include "MethodReflection.h"


namespace Beer
{
	class MethodReflection;
	class Class;

	#pragma pack(push, 1)
	class MonomorphicInlineCache
	{
	protected:
		MethodReflection* mMethod;

	public:
		INLINE void clear()
		{
			memset(this, 0, countSize());
		}

		INLINE MethodReflection* find(Class* klass, String* selector)
		{
			DBG_ASSERT(klass != NULL, BEER_WIDEN("Class is NULL"));
			if(mMethod == NULL)
			{
				mMethod = klass->findMethod(selector->c_str());
			}
			return mMethod;
		}

		INLINE static MonomorphicInlineCache* from(void* ptr)
		{
			return reinterpret_cast<MonomorphicInlineCache*>(ptr);
		}
		INLINE static uint16 countSize()
		{
			return sizeof(MonomorphicInlineCache);
		}
	};
	#pragma pack(pop)
};