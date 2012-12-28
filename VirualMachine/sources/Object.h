#pragma once
#include "prereq.h"
//#include "GarbageCollector.h"
//#include "ClassReflection.h"


/*
#define VIRTUAL_TABLE_SIZE sizeof(void*)


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define COLLECTED_OBJECT_SET_CHILDREN_COUNT(children_count)												\
	static const int CHILDREN_COUNT = (children_count);													\
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXTENDING_COLLECTED_OBJECT(children_count)														\
	public: COLLECTED_OBJECT_SET_CHILDREN_COUNT(children_count);										\
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXTENDING_COLLECTED_OBJECT_ADDING(added_children_count)											\
	EXTENDING_COLLECTED_OBJECT(CHILDREN_COUNT + added_children_count);									\
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXTENDING_COLLECTED_OBJECT_ADDING_0()															\
	EXTENDING_COLLECTED_OBJECT_ADDING(0);																\
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXTENDING_COLLECTED_OBJECT_ADDING_1(name1)														\
	public: enum {name1 = CHILDREN_COUNT + 0};															\
	EXTENDING_COLLECTED_OBJECT_ADDING(1);																\
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXTENDING_COLLECTED_OBJECT_ADDING_2(name1, name2)												\
	public: enum {name1 = CHILDREN_COUNT + 0, name2 = CHILDREN_COUNT + 1};								\
	EXTENDING_COLLECTED_OBJECT_ADDING(2);																\
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXTENDING_COLLECTED_OBJECT_ADDING_3(name1, name2, name3)										\
	public: enum {																						\
		name1 = CHILDREN_COUNT + 0,																		\
		name2 = CHILDREN_COUNT + 1,																		\
		name3 = CHILDREN_COUNT + 2																		\
	};																									\
	EXTENDING_COLLECTED_OBJECT_ADDING(3);																\
/////////////////////////////////////////////////////////////////////////////////////////////////////////
*/


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

		//COLLECTED_OBJECT_SET_CHILDREN_COUNT(0);

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
			DBG_ASSERT(!isInlineValue(), "Tried to get class of an inline value");
			return mClass;
		}

		// inline value
		INLINE bool isInlineValue() const { return (reinterpret_cast<InlineValueId>(this) & 1); }

		INLINE InlineValueId getInlineClassId()
		{
			// *NO* assert !!!
			//DBG_ASSERT(isInlineValue(), "Tried to get inline class id of a non inline value");
			return reinterpret_cast<InlineValueId>(this) & 15;
		}

		// children
		INLINE Object** getChildren() { return mChildren; }
		INLINE void setChildren(Object** value) { mChildren = value; }

		INLINE Object* getChild(uint32 index) { return mChildren[index]; }
		INLINE void setChild(uint32 index, Object* obj) { mChildren[index] = obj; }

		template <typename T>
		INLINE T* getChild(uint32 index) { return static_cast<T*>(getChild(index)); }

		// casts
		
		template <typename T>
		INLINE T* getInstance()
		{
			return static_cast<T*>(this);
		}
	};
	#pragma pack(pop)
};