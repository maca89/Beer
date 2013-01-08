#pragma once
#include "prereq.h"
#include "NativeThread.h"
#include "VirtualMachine.h"
#include "StackFrame.h"


namespace Beer
{
	class Thread : public NativeThread
	{
	protected:
		VirtualMachine* mVM;
		StackFrame* mLastFrame;
		WorkStack mStack;
		DynamicStack<StackFrame> mFrames;

	public:
		INLINE Thread(VirtualMachine* vm) : mVM(vm), mStack(1024), mFrames(50)
		{
			StackFrame frame(&mStack);
			mFrames.push(frame);
		}

		virtual ~Thread()
		{
		}

		INLINE WorkStack* getStack() { return &mStack; }

		INLINE StackFrame* openStackFrame()
		{
			StackFrame frame(getStackFrame());
			mFrames.push(frame);
			return getStackFrame();
		}

		INLINE bool hasStackFrame() { return mFrames.size() > 0; }

		INLINE void closeStackFrame() { mFrames.pop(); }

		INLINE StackFrame* getStackFrame() { return mFrames.topPtr(mFrames.topIndex()); }
	};
};