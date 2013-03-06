#pragma once
#include "prereq.h"


namespace Beer
{
	//struct GarbageCollector;
	//class Class;

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
		};


	protected:
		uint8 mGCFlag;
		uint8 mTypeFlag;
		Object** mChildren;

	public:
		// gc flag
		INLINE GCFlag getGCFlag() const { return static_cast<GCFlag>(mGCFlag); }
		INLINE void setGCFlag(GCFlag value) { mGCFlag = value; }

		// type flag
		INLINE TypeFlag getTypeFlag() const { return static_cast<TypeFlag>(mTypeFlag); }
		INLINE void setTypeFlag(TypeFlag value) { mTypeFlag = value; }
		
		// class
		template <typename T>
		INLINE void setClass(T* klass)
		{
			DBG_ASSERT(!isInlineValue(this), BEER_WIDEN("Tried to set class of an inline value"));
			setChild(1, klass);
		}
		
		template <typename T>
		INLINE T* getClass()
		{
			DBG_ASSERT(!isInlineValue(this), BEER_WIDEN("Tried to get class of an inline value"));
			return getChild<T>(1);
		}

		// inline value
		INLINE static bool isInlineValue(const Object* object)
		{
			if(object == NULL) return true; // just a workaround, TODO: get rid of
			return (reinterpret_cast<InlineValueId>(object) & 1);
		}

		// children count
		INLINE Object* getChildrenCount() { return getChild<Object>(0); }
		INLINE void setChildrenCount(Object* value) { return setChild(0, value); }

		// children
		INLINE Object** getChildren() { return mChildren; }
		INLINE void setChildren(Object** value) { mChildren = value; }

		INLINE Object* getChild(int64 index) const { return mChildren[index]; }
		INLINE void setChild(int64 index, Object* obj) { mChildren[index] = obj; }

		template <typename T>
		INLINE T* getChild(int64 index) const { return static_cast<T*>(getChild(index)); }

		// casts
		
		/*template <typename T>
		INLINE T* getInstance()
		{
			return static_cast<T*>(this);
		}*/
	};
	#pragma pack(pop)
};