#pragma once
#include "prereq.h"
#include "Object.h"
#include "String.h"
#include "Param.h"
#include "StackFrame.h"
#include "Bytecode.h"
#include "MiliTimer.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;

	class Method : public Object
	{
	public:
		typedef void(*Cb)();

		enum
		{
			FLAG_BYTECODE = 0x01,
			FLAG_INTERNAL = 0x02
		};

		enum
		{
			METHOD_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 1 // name
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
		
		// returns

		INLINE uint16 getReturnsCount() const
		{
			return mReturnsCount;
		}

		INLINE void setReturn(uint16 i, Param* value)
		{
			DBG_ASSERT(i < getReturnsCount(), BEER_WIDEN("Unable to add more returns"));
			setChild(METHOD_CHILDREN_COUNT + i, value);
		}
		
		INLINE Param* getReturn(uint16 i) const
		{
			DBG_ASSERT(i < getReturnsCount(), BEER_WIDEN("Unknown return"));
			return getChild<Param>(METHOD_CHILDREN_COUNT + i);
		}

		// params

		INLINE uint16 getParamsCount() const
		{
			return mParamsCount;
		}

		INLINE void setParam(uint16 i, Param* value)
		{
			DBG_ASSERT(i < getParamsCount(), BEER_WIDEN("Unable to add more params"));
			setChild(METHOD_CHILDREN_COUNT + getReturnsCount() + i, value);
		}

		INLINE Param* getParam(uint16 i)
		{
			DBG_ASSERT(i < getParamsCount(), BEER_WIDEN("Unknown param"));
			return getChild<Param>(METHOD_CHILDREN_COUNT + getReturnsCount() + i);
		}

		// max stack

		INLINE uint16 getMaxStack() const { return mMaxStack; }
		INLINE void setMaxStack(uint16 value) { mMaxStack = value; }

		// name
		
		INLINE String* getName() const
		{
			return getChild<String>(OBJECT_CHILDREN_COUNT);
		}

		INLINE void setName(String* value)
		{
			setChild(OBJECT_CHILDREN_COUNT, value);
		}

		// NativeMethod
		
		template <typename T>
		INLINE void setFunction(T fn)
		{
			setFunction(reinterpret_cast<Cb>(fn));
		}

		INLINE void setFunction(Cb fn)
		{
			markAsInternal();
			mFunction = fn;
		}

		// BytecodeMethod

		INLINE const Bytecode* getBytecode() const { return mBytecode; }
		
		INLINE void setBytecode(Bytecode* value)
		{
			markAsBytecode();
			mBytecode = value;
		}

		// calls

		INLINE Method* call(Thread* thread/*, StackFrame* frame*/)
		{
			if(isBytecode())
			{
				return mBytecode->call(thread/*, frame*/);
			}
			else
			{
				return runFunction(thread/*, frame*/);
			}
		}

	protected:
		Method* runFunction(Thread* thread/*, StackFrame* frame*/);

	private:
		INLINE ~Method()
		{
			// never called!
		}
	};
};