#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "StackFrame.h"


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

		static void BEER_CALL createInstance(Thread* thread/*, StackFrame* frame*/, StackRef<Class> receiver, StackRef<Float> ret);

		static void BEER_CALL init(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> ret);

		static void BEER_CALL operatorString(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<String> ret);
		static void BEER_CALL operatorInteger(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Integer> ret);

		static void BEER_CALL operatorAdd(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorSub(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorMul(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorDiv(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret);
		static void BEER_CALL operatorMinus(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> ret);

		static void BEER_CALL operatorEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmaller(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmallerEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreater(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreaterEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret);
	};
	
	class FloatClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};