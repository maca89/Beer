#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "IntegerClass.h"


namespace Beer
{
	class VirtualMachine;

	class Array : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();

	protected:
		int32 mSize;
		ClassReflection* mItemClass;
	
	public:
		INLINE int32 getSize() const
		{
			return mSize;//mHeader.childrenCount - CHILDREN_COUNT;
		}

		INLINE void setSize(int32 value)
		{
			mSize = value;
		}

		INLINE Object* getItem(int32 i)
		{
			return getChild<Object>(/*CHILDREN_COUNT +*/ i);
		}

		INLINE void setItem(int32 i, Object* obj)
		{
			//DBG_ASSERT(obj->getClass()->substituable(mItemClass), "Unexpected class"); // TODO: CLASS_ASSERT
			setChild(/*CHILDREN_COUNT +*/ i, obj);
		}

		INLINE ClassReflection* getItemClass()
		{
			return mItemClass;
		}

		INLINE void setItemClass(ClassReflection* klass)
		{
			mItemClass = klass;
		}

		NOINLINE void toString(VirtualMachine* vm, std::string& out);
	};

	class ArrayClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual uint32 getChildrenCount(Object* object)
		{
			return static_cast<Array*>(object)->getSize();
		}

		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc);
		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc);

		/*svirtual void dump(Object* object, std::stringstream& out)
		{
			out << object->getInstance<Array>()->getData();
		};*/
	};

	class ArrayClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};