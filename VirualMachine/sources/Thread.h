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
		Frames mFrames;
		WorkStack mStack;

	public:
		INLINE Thread(VirtualMachine* vm) : mVM(vm), mStack(1024)
		{
			StackFrame frame(&mStack);
			mFrames.push_back(frame);
		}

		virtual ~Thread()
		{
		}

		INLINE WorkStack* getStack() { return &mStack; }

		INLINE StackFrame* openStackFrame()
		{
			StackFrame frame(getStackFrame());
			mFrames.push_back(frame);
			return getStackFrame();
		}

		INLINE bool hasStackFrame() { return !mFrames.empty(); }

		INLINE void closeStackFrame() { mFrames.pop_back(); }

		INLINE StackFrame* getStackFrame() { return &mFrames.back(); }
	};
};