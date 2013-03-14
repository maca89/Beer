#pragma once
#include "prereq.h"
#include "Object.h"
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

		INLINE Class* getItemClass()
		{
			return mItemClass;
		}

		INLINE void setItemClass(Class* klass)
		{
			mItemClass = klass;
		}

		NOINLINE void toString(Thread* thread, string& out);

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Array> ret);

		static void BEER_CALL init(Thread* thread, StackRef<Array> receiver, StackRef<Integer> length, StackRef<Array> ret);
		static void BEER_CALL getLength(Thread* thread, StackRef<Array> receiver, StackRef<Integer> ret);
		static void BEER_CALL operatorGet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> ret);
		static void BEER_CALL operatorSet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> object);
	};

	class ArrayClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual void createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret);
		virtual void initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass);
	};
};