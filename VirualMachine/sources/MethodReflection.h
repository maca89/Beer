#pragma once
#include "prereq.h"
#include "Object.h"
#include "Selector.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;
	class ParamReflection;
	class Bytecode;

	class MethodReflection : public Object
	{
	public:
		typedef void(*Cb)();

		enum
		{
			FLAG_BYTECODE = 0x01,
			FLAG_INTERNAL = 0x02
		};

		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint8 mFlags;

		uint16 mNameCount;
		char_t* mName;
		
		uint16 mSelectorCount;
		char_t* mSelector;

		uint16 mReturnsCount;
		ParamReflection** mReturns;

		uint16 mParamsCount;
		ParamReflection** mParams;

		uint16 mMaxStack;
		////////////////////////////////////////////////////////////

		Cb mFunction;
		Bytecode* mBytecode;

	public:
		INLINE MethodReflection() : mFunction(NULL),mBytecode(NULL)
		{
		}

		INLINE ~MethodReflection()
		{
			// is never called!
		}

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

		INLINE uint16 getReturnsCount() const { return mReturnsCount; }
		INLINE void setReturn(uint16 i, ParamReflection* ret) { DBG_ASSERT(i < mReturnsCount, BEER_WIDEN("Unable to add more returns")); mReturns[i] = ret; }
		INLINE ParamReflection* getReturn(uint16 i) { DBG_ASSERT(i < mReturnsCount, BEER_WIDEN("Unknown return")); return mReturns[i]; }

		// params

		INLINE uint16 getParamsCount() const { return mParamsCount; }
		INLINE void setParam(uint16 i, ParamReflection* param) { DBG_ASSERT(i < mParamsCount, BEER_WIDEN("Unable to add more params")); mParams[i] = param; }
		INLINE ParamReflection* getParam(uint16 i) { DBG_ASSERT(i < mParamsCount, BEER_WIDEN("Unknown param")); return mParams[i]; }

		// max stack

		INLINE uint16 getMaxStack() const { return mMaxStack; }
		INLINE void setMaxStack(uint16 value) { mMaxStack = value; }

		// name

		INLINE const char_t* getName() const { return mName; }

		// selector

		INLINE const char_t* getSelector() const { return mSelector; }

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

		INLINE void call(VirtualMachine* vm, StackFrame* frame)
		{
			// TODO

			if(isBytecode())
			{
				runBytecode(vm, frame);
			}
			else
			{
				runFunction(vm, frame);
			}
		}

	protected:
		void runBytecode(VirtualMachine* vm, StackFrame* frame);
		void runFunction(VirtualMachine* vm, StackFrame* frame);
	};
};