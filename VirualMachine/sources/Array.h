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
		LengthData mSize;
		Class* mItemClass;
	
	public:
		INLINE LengthData getSize() const
		{
			return mSize;
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

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Array> ret);

		static void BEER_CALL init(Thread* thread, StackRef<Array> receiver, StackRef<Integer> length, StackRef<Array> ret);
		static void BEER_CALL getLength(Thread* thread, StackRef<Array> receiver, StackRef<Integer> ret);
		static void BEER_CALL operatorGet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> ret);
		static void BEER_CALL operatorSet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> object);

		static void BEER_CALL operatorString(Thread* thread, StackRef<Array> receiver, StackRef<String> ret);
	};

	class ArrayClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual void createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret);
		virtual void initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass);
	};
};