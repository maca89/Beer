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
		// TODO: garbaged
		LengthData mSize;
		Class* mItemClass;
	
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
			return getChild<Object>(OBJECT_CHILDREN_COUNT + i);
		}

		INLINE void setItem(LengthData i, Object* obj)
		{
			//DBG_ASSERT(obj->getClass()->substituable(mItemClass), BEER_WIDEN("Unexpected class")); // TODO: CLASS_ASSERT
			setChild(OBJECT_CHILDREN_COUNT + i, obj);
		}

		INLINE Class* getItemClass()
		{
			return mItemClass;
		}

		INLINE void setItemClass(Class* klass)
		{
			mItemClass = klass;
		}

		NOINLINE void toString(VirtualMachine* vm, string& out);
	};

	class ArrayClass : public Class
	{
	public:
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc);
	};

	class ArrayClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};