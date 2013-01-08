#pragma once
#include "prereq.h"
#include "MethodReflection.h"


namespace Beer
{
	class MethodReflection;
	class ClassReflection;

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

		INLINE MethodReflection* find(ClassReflection* klass, String* selector)
		{
			return find(klass, selector->c_str());
		}

		INLINE MethodReflection* find(ClassReflection* klass, const char_t* selector)
		{
			DBG_ASSERT(klass != NULL, BEER_WIDEN("Class is NULL"));
			if(mMethod == NULL)
			{
				mMethod = klass->findMethod(selector);
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