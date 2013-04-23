#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "Frame.h"


// unsigned 30bit precision [0, 1073741823]
#define BEER_INLINED_INTEGER_MODE_UNSIGNED 1

// signed 30bit precision [-536870912, +536870911]
#define BEER_INLINED_INTEGER_MODE_SIGNED 2

//
#define BEER_INLINED_INTEGER_MODE	BEER_INLINED_INTEGER_MODE_SIGNED


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

		
#if BEER_INLINED_INTEGER_MODE == BEER_INLINED_INTEGER_MODE_UNSIGNED
		INLINE static bool canBeInlineValue(IntegerData data)
		{
			IntegerData msb = (data >> (sizeof(Integer*) * 8 - SignatureBits));
			return msb == 0;
		}

		INLINE static Integer* makeInlineValue(IntegerData data)
		{
			return reinterpret_cast<Integer*>((data << SignatureBits) | 1);
		}

		INLINE static IntegerData getInlineValue(const Integer* data)
		{
			return reinterpret_cast<IntegerData>(data) >> SignatureBits;
		}
#elif BEER_INLINED_INTEGER_MODE == BEER_INLINED_INTEGER_MODE_SIGNED
		INLINE static bool canBeInlineValue(IntegerData data)
		{
			IntegerData msb = (data >> (sizeof(Integer*) * 8 - SignatureBits - 1));
			return msb == 0 || msb == -1; // (decimal)-1 == (binary)111...111
		}

		INLINE static Integer* makeInlineValue(IntegerData data)
		{
			return reinterpret_cast<Integer*>((data << SignatureBits) | 1);
		}

		INLINE static IntegerData getInlineValue(const Integer* data)
		{
			return reinterpret_cast<IntegerData>(data) >> SignatureBits;
		}
#endif // BEER_INLINED_INTEGER_MODE

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);

		static void BEER_CALL init(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> ret1);
		
		static void BEER_CALL operatorFloat(Thread* thread, StackRef<Integer> receiver, StackRef<Float> ret);
		static void BEER_CALL operatorString(Thread* thread, StackRef<Integer> receiver, StackRef<String> ret);

		static void BEER_CALL operatorAdd(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorSub(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorMul(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorMod(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret);
		static void BEER_CALL operatorDiv(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Float> ret);

		static void BEER_CALL operatorMinus(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> ret);

		static void BEER_CALL operatorEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmaller(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmallerEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreater(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreaterEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret);

		static void BEER_CALL abs(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> ret);

	};
	
	class IntegerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};