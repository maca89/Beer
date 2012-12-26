#pragma once
#include "prereq.h"


namespace Beer
{
	class MethodReflection;
	class ClassReflection;

	#pragma pack(push, 1)
	class InlineCache
	{
	public:
		struct CachedMethod
		{
			ClassReflection* klass;
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

		NOINLINE MethodReflection* find(ClassReflection* klass, const char* selector, uint16 methodsLength);

		INLINE static InlineCache* from(void* ptr)
		{
			return reinterpret_cast<InlineCache*>(ptr);
		}
		INLINE static uint16 countSize(uint16 methods)
		{
			return /*sizeof(uint16) +*/ sizeof(CachedMethod) * methods;
		}
	};
	#pragma pack(pop)
};