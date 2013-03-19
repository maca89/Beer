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
			TYPE_REF = 0,
			TYPE_VALUE = 1,
			TYPE_FWD_PTR = 2,
		};

		typedef uint8 InlineValueId;

		enum
		{
			// children order:
			// #1 children count
			// #2 class
			OBJECT_CHILDREN_COUNT = 2,
			OBJECT_METHODS_COUNT = 6,

			CHILD_ID_CLASS = 1,
		};


	protected:
		uint8 mGCFlag;
		uint8 mTypeFlag;
		Object** mChildren;

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

		// deprecated
		INLINE Object** getChildren() { return mChildren; } // deprecated
		INLINE void setChildren(Object** value) { mChildren = value; } // deprecated

		// shortcuts
		static void getChild(Thread* thread, StackRef<Object> object, StackRef<Object> ret, int64 index);
		static void setChild(Thread* thread, StackRef<Object> object, StackRef<Object> child, int64 index);

		// methods

		static void BEER_CALL init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret);
		static void BEER_CALL operatorString(Thread* thread, StackRef<Object> receiver, StackRef<String> ret);

		// TODO: rename to getType, now it is confused with other methods
		static void BEER_CALL setClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> param);
		static void BEER_CALL getClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret);

		static void BEER_CALL getChild(Thread* thread, StackRef<Object> receiver, StackRef<Integer> index, StackRef<Object> ret);
		static void BEER_CALL setChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> child, StackRef<Integer> index);
		
		// deprecated
		friend class GenerationalGC;
	};
	#pragma pack(pop)
};