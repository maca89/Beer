#pragma once
#include "prereq.h"
#include "String.h"


namespace Beer
{
	class Method;
	class Class;

	#pragma pack(push, 1)
	class PolymorphicInlineCache
	{
	public:
		struct CachedMethod
		{
			Class* klass;
			Method* method;
		};

	protected:
		CachedMethod mMethods;

	public:
		INLINE void clear(uint16 methodsLength)
		{
			memset(this, 0, countSize(methodsLength));
		}

		INLINE CachedMethod* getMethods()
		{
			return &mMethods;
		}

		NOINLINE Method* find(Thread* thread, Class* klass, String* selector, uint16 methodsLength);

		INLINE static PolymorphicInlineCache* from(void* ptr)
		{
			return reinterpret_cast<PolymorphicInlineCache*>(ptr);
		}
		INLINE static uint16 countSize(uint16 methods)
		{
			return /*sizeof(uint16) +*/ sizeof(CachedMethod) * methods;
		}
	};
	#pragma pack(pop)
};