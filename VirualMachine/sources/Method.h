#pragma once
#include "prereq.h"
#include "Object.h"
#include "String.h"
#include "Param.h"
#include "Frame.h"
#include "Bytecode.h"
#include "MiliTimer.h"


namespace Beer
{
	class VirtualMachine;
	class Frame;

	class Method : public Object
	{
	public:
		enum
		{
			FLAG_BYTECODE = 0x01,
			FLAG_NATIVE = 0x02
		};

		enum
		{
			METHOD_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT,
		};

		// children order:
		// # name
		// # returns size		// TODO
		// # returns array
		// # params size		// TODO
		// # params array

	protected:
		uint8 mFlags;

		uint16 mMaxStack;
		Cb mFunction;
		Bytecode* mBytecode;
		float64 mTimeSpent;

		uint16 mReturnsCount;
		uint16 mParamsCount;

		String* mName;
		Pool* mPool;

	public:
		// time spent
		INLINE void addTimeSpent(float64 value) { mTimeSpent += value; }
		INLINE void setTimeSpent(float64 value) { mTimeSpent = value; }
		INLINE float64 getTimeSpent() const { return mTimeSpent; }

		// flags
		
		INLINE uint8 getFlags() const { return mFlags; }
		INLINE void setFlags(uint8 value) { mFlags = value; }
		
		INLINE void markBytecode() { markFlag(FLAG_BYTECODE); }
		INLINE void unmarkBytecode() { unmarkFlag(FLAG_BYTECODE); }
		INLINE bool isBytecode() const { return hasFlag(FLAG_BYTECODE); }
		
		INLINE void markNative() { markFlag(FLAG_NATIVE); }
		INLINE void unmarkNative() { unmarkFlag(FLAG_NATIVE); }
		INLINE bool isNative() const { return hasFlag(FLAG_NATIVE); }

		INLINE bool hasFlag(uint8 n) const { return (mFlags & n) == n; }
		INLINE void markFlag(uint8 n) { mFlags |= n; }
		INLINE void unmarkFlag(uint8 n) { markFlag(n); mFlags ^= n; }

		//

		String* getName() const;
		void setName(String* value);

		uint16 getReturnsCount() const;
		void setReturnsCount(uint16 value);

		uint16 getParamsCount() const;
		void setParamsCount(uint16 value);

		uint16 getMaxStack() const;
		void setMaxStack(uint16 value);

		INLINE void setFunction(Cb fn)
		{
			unmarkBytecode();
			markNative();
			mFunction = fn;
		}
		
		INLINE void setBytecode(Bytecode* value)
		{
			unmarkNative();
			markBytecode();
			mBytecode = value;
		}
		
		INLINE Bytecode* getBytecode() { return mBytecode; }

		// calls

		INLINE void call(Thread* thread)
		{
			if(isBytecode())
			{
				mBytecode->call(thread);
			}
			else
			{
				runFunction(thread);
			}
		}

		void loadFromPool(Thread* thread, uint16 index, StackRef<Object> ret);
		uint16 storeToPool(Thread* thread, StackRef<Object> object);
		INLINE void updateAtPool(Thread* thread, uint16 index, StackRef<Object> object);
		void createPool(Thread* thread, uint16 length);

		// methods
		
		static void BEER_CALL getName(Thread* thread, StackRef<Method> receiver, StackRef<String> ret);
		static void BEER_CALL setName(Thread* thread, StackRef<Method> receiver, StackRef<String> value);
		
		static void BEER_CALL getReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret);
		static void BEER_CALL setReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value);
		
		static void BEER_CALL getParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret);
		static void BEER_CALL setParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value);

		// shorcuts

		static void getParam(Thread* thread, StackRef<Method> receiver, StackRef<Param> ret, Integer::IntegerData index);

		// traversers
		static void MethodTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance);

	protected:
		void runFunction(Thread* thread);
	};

	// inline definitions

	INLINE String* Method::getName() const
	{
		return mName;
	}

	INLINE void Method::setName(String* value)
	{
		mName = value;
	}

	INLINE uint16 Method::getReturnsCount() const
	{
		return mReturnsCount;
	}

	INLINE void Method::setReturnsCount(uint16 value)
	{
		mReturnsCount = value;
	}

	INLINE uint16 Method::getParamsCount() const
	{
		return mParamsCount;
	}

	INLINE void Method::setParamsCount(uint16 value)
	{
		mParamsCount = value;
	}

	INLINE uint16 Method::getMaxStack() const
	{
		return mMaxStack;
	}

	INLINE void Method::setMaxStack(uint16 value)
	{
		mMaxStack = value;
	}
};