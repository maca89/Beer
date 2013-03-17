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
			FLAG_INTERNAL = 0x02
		};

		enum
		{
			METHOD_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 1, // name

			CHILD_ID_METHOD_NAME = OBJECT_CHILDREN_COUNT,
		};

		// children order:
		// # name
		// # returns size		// TODO
		// # returns array
		// # params size		// TODO
		// # params array

		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint8 mFlags;

		// TODO: garbaged
		uint16 mReturnsCount;
		uint16 mParamsCount;
		////////////////////////////////////////////////////////////

	protected:
		uint16 mMaxStack;
		Cb mFunction;
		Bytecode* mBytecode;
		float64 mTimeSpent;

	public:
		INLINE Method() : mFunction(NULL), mBytecode(NULL), mTimeSpent(0)
		{
		}

		// time spent
		INLINE void addTimeSpent(float64 value) { mTimeSpent += value; }
		INLINE void setTimeSpent(float64 value) { mTimeSpent = value; }
		INLINE float64 getTimeSpent() const { return mTimeSpent; }

		// flags
		
		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }
		
		INLINE void markAsBytecode() { markFlag(FLAG_BYTECODE); }
		INLINE bool isBytecode() const { return hasFlag(FLAG_BYTECODE); }
		
		INLINE void markAsInternal() { markFlag(FLAG_INTERNAL); }
		INLINE bool isInternal() const { return hasFlag(FLAG_INTERNAL); }

		INLINE bool hasFlag(uint8 n) const { return (mFlags & n) == n; }
		INLINE void markFlag(uint8 n) { mFlags |= n; }

		INLINE uint16 getReturnsCount() const { return mReturnsCount; }
		INLINE uint16 getParamsCount() const { return mParamsCount; }

		INLINE uint16 getMaxStack() const { return mMaxStack; }
		INLINE void setMaxStack(uint16 value) { mMaxStack = value; }

		INLINE void setFunction(Cb fn)
		{
			markAsInternal();
			mFunction = fn;
		}
		
		INLINE void setBytecode(Bytecode* value)
		{
			markAsBytecode();
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

		// methods
		
		static void BEER_CALL getName(Thread* thread, StackRef<Method> receiver, StackRef<String> ret);
		static void BEER_CALL setName(Thread* thread, StackRef<Method> receiver, StackRef<String> value);
		
		static void BEER_CALL getReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret);
		static void BEER_CALL setReturn(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value);
		
		static void BEER_CALL getParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> ret);
		static void BEER_CALL setParam(Thread* thread, StackRef<Method> receiver, StackRef<Integer> index, StackRef<Param> value);

		// shorcuts

		static void BEER_CALL getParam(Thread* thread, StackRef<Method> receiver, StackRef<Param> ret, Integer::IntegerData index);

	protected:
		void runFunction(Thread* thread);
	};
};