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
	public:
		//typedef Integer::IntegerData ValueData;
		typedef Integer::IntegerData LengthData;

	protected:
		LengthData mSize;
		ClassReflection* mItemClass;
	
	public:
		INLINE LengthData getSize() const
		{
			DBG_ASSERT(mSize >= 0, BEER_WIDEN("Array size is smaller than zero"));
			return mSize;//mHeader.childrenCount - CHILDREN_COUNT;
		}

		INLINE void setSize(LengthData value)
		{
			mSize = value;
		}

		INLINE Object* getItem(LengthData i)
		{
			return getChild<Object>(i);
		}

		INLINE void setItem(LengthData i, Object* obj)
		{
			//DBG_ASSERT(obj->getClass()->substituable(mItemClass), BEER_WIDEN("Unexpected class")); // TODO: CLASS_ASSERT
			setChild(i, obj);
		}

		INLINE ClassReflection* getItemClass()
		{
			return mItemClass;
		}

		INLINE void setItemClass(ClassReflection* klass)
		{
			mItemClass = klass;
		}

		NOINLINE void toString(VirtualMachine* vm, string& out);
	};

	class ArrayClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual uint64 getChildrenCount(Object* object)
		{
			return static_cast<Array*>(object)->getSize();
		}

		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc);
		virtual Object* cloneShallow(VirtualMachine* vm, Object* object, StackFrame* frame, GarbageCollector* gc);

		/*svirtual void dump(Object* object, stringstream& out)
		{
			out << object->getInstance<Array>()->getData();
		};*/
	};

	class ArrayClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};