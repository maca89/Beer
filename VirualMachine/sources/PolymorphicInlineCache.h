#pragma once
#include "prereq.h"
#include "String.h"


namespace Beer
{
	class Method;
	class Class;

	//#pragma pack(push, 1)
	class PolymorphicCache : public Object
	{
	public:
		enum
		{
			POLYCACHE_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 2,
			CHILD_ID_LOCKS = OBJECT_CHILDREN_COUNT,
			CHILD_ID_METHODS = OBJECT_CHILDREN_COUNT + 1
		};

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

		INLINE static PolymorphicCache* from(void* ptr)
		{
			return reinterpret_cast<PolymorphicCache*>(ptr);
		}
		INLINE static uint16 countSize(uint16 methods)
		{
			return /*sizeof(PolymorphicCache) +*/ sizeof(CachedMethod) * methods;
		}
	};
	//#pragma pack(pop)
};