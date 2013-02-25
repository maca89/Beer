#pragma once
#include "prereq.h"
#include "Method.h"


namespace Beer
{
	class Method;
	class Class;

	#pragma pack(push, 1)
	class MonomorphicInlineCache
	{
	protected:
		Method* mMethod;

	public:
		INLINE void clear()
		{
			memset(this, 0, countSize());
		}

		INLINE Method* find(Class* klass, String* selector)
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