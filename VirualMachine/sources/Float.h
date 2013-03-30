#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "Frame.h"


namespace Beer
{
	class VirtualMachine;
	class Integer;
	class String;
	class Boolean;

	class Float : public Object
	{
	public:
		typedef float64 FloatData;

	protected:
		FloatData mData;

	public:
		INLINE FloatData getData() const
		{
			return mData;
		}

		INLINE void setData(FloatData data)
		{
			mData = data;
		}

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Float> ret);

		static void BEER_CALL init(Thread* thread, StackRef<Float> receiver, StackRef<Float> ret);

		static void BEER_CALL operatorString(Thread* thread, StackRef<Float> receiver, StackRef<String> ret);
		static void BEER_CALL operatorInteger(Thread* thread, StackRef<Float> receiver, StackRef<Integer> ret);

		static void BEER_CALL operatorAdd(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorSub(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorMul(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorDiv(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorMinus(Thread* thread, StackRef<Float> receiver, StackRef<Float> ret);

		static void BEER_CALL operatorEqual(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmaller(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmallerEqual(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreater(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreaterEqual(Thread* thread, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
	};
	
	class FloatClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};