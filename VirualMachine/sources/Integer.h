#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "Frame.h"


namespace Beer
{
	class VirtualMachine;
	class Float;
	class String;
	class Boolean;

	class Integer : public Object
	{
	public:
		typedef int64 IntegerData;
		static const int SignatureBits = 2;

		static const int64 MinValue = INT64_MIN_VALUE;
		static const int64 MaxValue = INT64_MAX_VALUE;

	protected:
		IntegerData mData;

	public:
		INLINE IntegerData getData() const
		{
			if(isInlineValue(this))
			{
				return getInlineValue(this);
			}
			return mData;
		}

		INLINE void setNonInlineValue(IntegerData data)
		{
			mData = data;
		}

		NOINLINE void toString(string& out)
		{
			stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static bool canBeInlineValue(IntegerData data)
		{
			// TODO: negative numbers
			return (data >> (sizeof(IntegerData) * 8 - SignatureBits)) == 0;
		}

		INLINE static Integer* makeInlineValue(IntegerData data)
		{
			return reinterpret_cast<Integer*>((data << SignatureBits) | 1);
		}

		INLINE static IntegerData getInlineValue(const Integer* data)
		{
			return reinterpret_cast<IntegerData>(data) >> SignatureBits;
		}

		static void BEER_CALL createInstance(Thread* thread/*, Frame* frame*/, StackRef<Class> receiver, StackRef<Integer> ret);

		static void BEER_CALL init(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> ret1);
		
		static void BEER_CALL operatorFloat(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Float> ret);
		static void BEER_CALL operatorString(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<String> ret);

		static void BEER_CALL operatorAdd(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorSub(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorMul(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorMod(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorDiv(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Float> ret);

		static void BEER_CALL operatorMinus(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> ret);

		static void BEER_CALL operatorEqual(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmaller(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmallerEqual(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreater(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreaterEqual(Thread* thread/*, Frame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);

	};
	
	class IntegerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};