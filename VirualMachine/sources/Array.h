#pragma once
#include "prereq.h"
#include "Object.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "Integer.h"


namespace Beer
{
	class VirtualMachine;
	class Class;

	class Array : public Object
	{
	public:
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

		static void BEER_CALL createInstance(Thread* thread, /*StackFrame* frame, */StackRef<Class> receiver, StackRef<Array> ret);

		static void BEER_CALL init(Thread* thread, /*StackFrame* frame, */StackRef<Array> receiver, StackRef<Integer> length, StackRef<Array> ret);
		static void BEER_CALL getLength(Thread* thread, /*StackFrame* frame, */StackRef<Array> receiver, StackRef<Integer> ret);
		static void BEER_CALL operatorGet(Thread* thread, /*StackFrame* frame, */StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> ret);
		static void BEER_CALL operatorSet(Thread* thread, /*StackFrame* frame, */StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> object);
	};

	class ArrayClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};