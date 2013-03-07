#pragma once
#include "prereq.h"
#include "StackFrame.h"


namespace Beer
{
	//struct GarbageCollector;
	//class Class;
	class Thread;
	class Integer;

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
		};

		typedef uint8 InlineValueId;

		enum
		{
			// children order:
			// #1 children count
			// #2 class
			OBJECT_CHILDREN_COUNT = 2,


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
		
		// deprecated
		template <typename T>
		INLINE void setClass(T* klass)
		{
			DBG_ASSERT(!isInlineValue(this), BEER_WIDEN("Tried to set class of an inline value"));
			_setChild(1, klass);
		}
		
		template <typename T>
		INLINE T* getClass()
		{
			DBG_ASSERT(!isInlineValue(this), BEER_WIDEN("Tried to get class of an inline value"));
			return _getChild<T>(1);
		}

		// inline value
		INLINE static bool isInlineValue(const Object* object)
		{
			return (reinterpret_cast<InlineValueId>(object) & 1);
		}

		// deprecated
		INLINE Object** getChildren() { return mChildren; } // deprecated
		INLINE void setChildren(Object** value) { mChildren = value; } // deprecated

		// deprecated
		INLINE Object* _getChild(int64 index) const { return mChildren[index]; }
		INLINE void _setChild(int64 index, Object* obj) { mChildren[index] = obj; }

		template <typename T>
		INLINE T* _getChild(int64 index) const { return static_cast<T*>(_getChild(index)); }

		// methods

		static void getChild(Thread* thread, StackRef<Object> object, StackRef<Object> ret, int64 index);
		static void setChild(Thread* thread, StackRef<Object> object, StackRef<Object> child, int64 index);

		static void BEER_CALL init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret);

		static void BEER_CALL setClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> param);
		static void BEER_CALL getClass(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret);

		static void BEER_CALL getChild(Thread* thread, StackRef<Object> receiver, StackRef<Integer> index, StackRef<Object> ret);
		static void BEER_CALL setChild(Thread* thread, StackRef<Object> receiver, StackRef<Object> child, StackRef<Integer> index);

		static Object* invokeGetChild(Thread* thread, Object* object, int64 index);
		static void invokeSetChild(Thread* thread, Object* object, Object* child, int64 index);
		
		// deprecated
		friend class GenerationalGC;
	};
	#pragma pack(pop)
};