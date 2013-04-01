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

	#pragma pack(push, 1)
	class Object
	{
	public:
		enum GCFlag
		{
			GC_WHITE = 0,
			GC_GRAY = 1,
			GC_BLACK = 2,
		};

		enum TypeFlag
		{
			TYPE_DIRECT_PTR = 0,
			TYPE_FWD_PTR = 1,
		};

		typedef uint8 InlineValueId;

		enum
		{
			// children order:
			// #1 children count
			// #2 class
			OBJECT_CHILDREN_COUNT = 0,
			OBJECT_METHODS_COUNT = 6,

			//CHILD_ID_CLASS = 1,
		};

		friend class Class;


	protected:
		Class* mType;
		uint32 mStaticSize; // temporary workaround, TODO

		// GC Header, TODO: move before Object
		uint8 mGCFlag;
		uint8 mTypeFlag;
		//Object* mIdentity; // fwd_ptr, direct_ptr
		//Object** mChildren;

	public:
		// deprecated
		INLINE GCFlag getGCFlag() const { return static_cast<GCFlag>(mGCFlag); }
		INLINE void setGCFlag(GCFlag value) { mGCFlag = value; }

		// deprecated
		INLINE TypeFlag getTypeFlag() const { return static_cast<TypeFlag>(mTypeFlag); }
		INLINE void setTypeFlag(TypeFlag value) { mTypeFlag = value; }
		
		// inline value
		INLINE static bool isInlineValue(const Object* object)
		{
			return (reinterpret_cast<InlineValueId>(object) & 1);
		}

		INLINE void setStaticSize(uint32 value) { mStaticSize = value; }
		INLINE uint32 getStaticSize() { return mStaticSize; }

		void setType(Class* value);
		Class* getType();
		
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
	#pragma pack(pop)


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
	
	INLINE Class* Object::getType()
	{
		return mType;
	}

	INLINE void* Object::getDynamicDataStart()
	{
		return getDynamicDataStart(mStaticSize);
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
};