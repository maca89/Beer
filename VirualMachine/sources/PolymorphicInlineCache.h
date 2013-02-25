#pragma once
#include "prereq.h"
#include "StringClass.h"


namespace Beer
{
	class MethodReflection;
	class Class;

	#pragma pack(push, 1)
	class PolymorphicInlineCache
	{
	public:
		struct CachedMethod
		{
			Class* klass;
			MethodReflection* method;
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

		INLINE MethodReflection* find(Class* klass, String* selector, uint16 methodsLength)
		{
			return find(klass, selector->c_str(), methodsLength);
		}

		NOINLINE MethodReflection* find(Class* klass, const char_t* selector, uint16 methodsLength);

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