#pragma once
#include "prereq.h"
//#include "StackRef.h"


namespace Beer
{
	//struct GarbageCollector;
	//class Class;
	class Thread;
	class Integer;

	template <typename T>
	class StackRef;

	//#pragma pack(push, 1)
	class Object
	{
	public:
		typedef uint8 InlineValueId;

		enum
		{
			// children order:
			// #1 children count
			// #2 class
			OBJECT_CHILDREN_COUNT = 0,
			OBJECT_METHODS_COUNT = 3, // createInstance, init, operatorString, TODO: cloneShallow, cloneDeep, getType

			//CHILD_ID_CLASS = 1,
		};

		friend class Class;


	protected:
		Class* mType;
		uint32 mStaticSize; // temporary workaround, TODO

	public:

		INLINE static void initObject(Object* obj, uint32 size, uint32 staticSize)
		{
			memset(obj, 0, size);
			obj->setStaticSize(staticSize);
		}

		// inline value
		INLINE static bool isInlineValue(const Object* object)
		{
			return (reinterpret_cast<InlineValueId>(object) & 1);
		}

		void setStaticSize(uint32 value);
		uint32 getStaticSize() const;

		void setType(Class* value);
		Class* getType() const;
		
		void* getDynamicDataStart();
		void* getDynamicDataStart(uint32 staticSize);

		Object** getChildren();
		Object** getChildren(uint32 staticSize);

		void setChild(int64 index, Object* child);
		Object* getChild(int64);
		
		static bool hasFlag(uint8 flags, uint8 n);
		static void markFlag(uint8& flags, uint8 n);
		static void unmarkFlag(uint8& flags, uint8 n);

		// shortcuts
		static void getChild(Thread* thread, StackRef<Object> object, int64 index, StackRef<Object> ret);
		static void setChild(Thread* thread, StackRef<Object> object, int64 index, StackRef<Object> child);

		// methods

		static void BEER_CALL init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret);
		static void BEER_CALL operatorString(Thread* thread, StackRef<Object> receiver, StackRef<String> ret);

		static void BEER_CALL setType(Thread* thread, StackRef<Object> receiver, StackRef<Class> param);
		static void BEER_CALL getType(Thread* thread, StackRef<Object> receiver, StackRef<Class> ret);
	};
	//#pragma pack(pop)


	// inlined methods

	INLINE bool Object::hasFlag(uint8 flags, uint8 n)
	{
		return (flags & n) == n;
	}
	
	INLINE void Object::markFlag(uint8& flags, uint8 n)
	{
		flags |= n;
	}
	
	INLINE void Object::unmarkFlag(uint8& flags, uint8 n)
	{
		Object::markFlag(flags, n);
		flags ^= n;
	}

	INLINE void Object::setType(Class* value)
	{
		mType = value;
	}
	
	INLINE Class* Object::getType() const
	{
		return mType;
	}

	INLINE void* Object::getDynamicDataStart()
	{
		return getDynamicDataStart(getStaticSize());
	}

	INLINE void* Object::getDynamicDataStart(uint32 staticSize)
	{
		return reinterpret_cast<byte*>(this) + staticSize;
	}

	INLINE Object** Object::getChildren()
	{
		return reinterpret_cast<Object**>(getDynamicDataStart());
	}

	INLINE Object** Object::getChildren(uint32 staticSize)
	{
		return reinterpret_cast<Object**>(getDynamicDataStart(staticSize));
	}

	INLINE void Object::setChild(int64 index, Object* child)
	{
		getChildren()[index] = child;
	}

	INLINE Object* Object::getChild(int64 index)
	{
		return getChildren()[index];
	}

	INLINE void Object::setStaticSize(uint32 value)
	{
		mStaticSize = value;
	}

	INLINE uint32 Object::getStaticSize() const
	{
		return mStaticSize;
	}

/*#include "Class.h"

	NOINLINE uint32 Object::getStaticSize()
	{
		if(m_StaticSize != getType()->getInstanceStaticSize())
		{
			int a = 0;
		}
		return m_StaticSize;
	}*/
};