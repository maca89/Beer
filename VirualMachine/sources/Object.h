#pragma once
#include "prereq.h"


namespace Beer
{
	struct GarbageCollector;
	class ClassReflection;

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
			TYPE_VALUE = 1
		};

		typedef uint8 InlineValueId;


	protected:
		uint8 mGCFlag;
		uint8 mTypeFlag;
		ClassReflection* mClass;
		Object** mChildren;

	public:
		// gc flag
		INLINE GCFlag getGCFlag() const { return static_cast<GCFlag>(mGCFlag); }
		INLINE void setGCFlag(GCFlag value) { mGCFlag = value; }

		// type flag
		INLINE TypeFlag getTypeFlag() const { return static_cast<TypeFlag>(mTypeFlag); }
		INLINE void setTypeFlag(TypeFlag value) { mTypeFlag = value; }
		
		// class
		INLINE void setClass(ClassReflection* klass) { mClass = klass; }

		INLINE ClassReflection* getClass()
		{
			DBG_ASSERT(!isInlineValue(this), BEER_WIDEN("Tried to get class of an inline value"));
			return mClass;
		}

		// inline value
		INLINE static bool isInlineValue(const Object* object) { return (reinterpret_cast<InlineValueId>(object) & 1); }

		// children
		INLINE Object** getChildren() { return mChildren; }
		INLINE void setChildren(Object** value) { mChildren = value; }

		INLINE Object* getChild(int64 index) { return mChildren[index]; }
		INLINE void setChild(int64 index, Object* obj) { mChildren[index] = obj; }

		template <typename T>
		INLINE T* getChild(int64 index) { return static_cast<T*>(getChild(index)); }

		// casts
		
		template <typename T>
		INLINE T* getInstance()
		{
			return static_cast<T*>(this);
		}
	};
	#pragma pack(pop)
};