#include "stdafx.h"
#include "TaskContext.h"
#include "StackRef.h"
#include "Method.h"
#include "Frame.h"


using namespace Beer;

TaskContext::TaskContext()
	: mHeap(NULL), mRootFrame(NULL), mTopFrame(NULL), mFramesCount(0)
{
	
}

TaskContext::~TaskContext()
{
}

void TaskContext::init(Heap* heap)
{
	mRootFrame = NULL;
	mTopFrame = NULL;
	mHeap = heap;

	// just a temporary solution, TODO
	
	mRootFrame = allocFrame(NULL, 250, 0);
	Frame* frame = allocFrame(NULL, 30, 2);
	//frame->setFrameOffset(2);
	frame->stackPush(); // simulate receiver
	frame->stackPush(); // simulate method
	mRootFrame->stackPush(frame);
	fetchTopFrame();
	mFramesCount++;
}

Frame* TaskContext::allocFrame(Frame* previousFrame, uint32 stackSize, uint32 argsCount)
{
	Frame* frame = NULL;

	if(!mTopFrame || (frame = mTopFrame->pushFrame(argsCount, stackSize)) == NULL)
	{
		uint32 frameLength = sizeof(Frame) + (stackSize * sizeof(Object*)) + DEFAULT_FRAME_SPACE + sizeof(Frame*);
		frame = mHeap->alloc<Frame>(frameLength, 0);
		if(frame == NULL)
		{
			throw NotEnoughMemoryException(BEER_WIDEN("Not enough memory to allocate Frame"));
		}

		void* bp = reinterpret_cast<byte*>(frame) + frameLength - sizeof(Frame*);
		*reinterpret_cast<Frame**>(bp) = previousFrame;
		new(frame) Frame(bp, argsCount, stackSize, DEFAULT_FRAME_SPACE);
		frame->markHeapAllocated();
		//cout << "frame " << frame << " allocated on heap\n";
	}
	else
	{
		//cout << "frame " << frame << " allocated on previous frame stack\n";
	}

	return frame;
}

void TaskContext::discardFrame(Frame* previousFrame, Frame* currentFrame)
{
	if(previousFrame == NULL)
	{
		//cout << "frame " << currentFrame << " not discarded, previous frame is NULL\n";
		return; // TODO???
	}

	if(previousFrame->popFrame(currentFrame))
	{
		//cout << "frame " << currentFrame << " discarded\n";
	}
	else
	{
		//cout << "frame " << currentFrame << " will be collected by GC\n";
	}
}

Frame* TaskContext::getPreviousFrame()
{
	if(mRootFrame->stackLength() > 1)
	{
		return mRootFrame->stackTop<Frame>(mRootFrame->stackTopIndex() - 1);
	}
	
	return NULL;
}

Frame* TaskContext::openFrame()
{
	Frame* oldFrame = getFrame();
	StackRef<Method> method(oldFrame, oldFrame->stackTopIndex());

	uint16 stackSize = 25;
	uint16 paramsCount = 0;
	uint16 returnsCount = 0;

	if(!method.isNull())
	{
		paramsCount = method->getParamsCount();
		returnsCount = method->getReturnsCount();
		stackSize = method->getMaxStack() + 5;
	}

	Frame* newFrame = allocFrame(oldFrame, stackSize, returnsCount + paramsCount + 2); // +2: receiver, method

	if(!newFrame->isContinuous())
	{
		// make space for returns, if/else is an optimization
		if(returnsCount == 1) newFrame->stackPush();
		else newFrame->stackMoveTop(returnsCount);

		// copy params
		for(uint16 i = paramsCount; i >= 1; i--) // *BEWARE* index starts at this position to skip the method on stack!
		{
			newFrame->stackPush(oldFrame->stackTop(oldFrame->stackTopIndex() - i - 1)); // -1 for receiver
		}
	
		//mVM->getDebugger()->printFrameStack(this, oldFrame);

		newFrame->stackPush(oldFrame->stackTop(oldFrame->stackTopIndex() - 1)); // copy receiver
		newFrame->stackPush(oldFrame->stackTop(oldFrame->stackTopIndex())); // copy method
	}

	mRootFrame->stackPush(newFrame);
	fetchTopFrame();
	mFramesCount++;
	return getFrame();
}

void TaskContext::closeFrame()
{
	Frame* currentFrame = getFrame();
	Frame* previousFrame = getPreviousFrame();

	StackRef<Method> method(currentFrame, Frame::INDEX_METHOD);

	uint16 paramsCount = 0;
	uint16 returnsCount = 0;

	if(!method.isNull())
	{
		//DBG_ASSERT(method.get()->getType() == NULL, BEER_WIDEN("Method's type should be NULL"));
		paramsCount = method->getParamsCount();
		returnsCount = method->getReturnsCount();
	}

	if(!currentFrame->isContinuous())
	{
	
		if(!method.isNull())
		{
			paramsCount = method->getParamsCount();
			returnsCount = method->getReturnsCount();
		}

		if(previousFrame)
		{
			// clean previous frame
			previousFrame->stackMoveTop(-(paramsCount + returnsCount + 2));
	
			// push returns on previous frame
			for(int32 i = returnsCount; i >= 1; i--)
			{
				previousFrame->stackPush(currentFrame->stackTop(-paramsCount - i - 2));
			}
		}
	}
	else if(previousFrame)
	{
		// clean previous frame
		previousFrame->stackMoveTop(-(paramsCount + 2)); // returns will stay
	}

#ifdef BEER_STACK_DEBUGGING
	// clean current frame
	currentFrame->stackMoveTop(-(paramsCount + returnsCount + 2));
#endif // BEER_STACK_DEBUGGING

	discardFrame(previousFrame, currentFrame);
	mRootFrame->stackPop();
	mTopFrame = previousFrame;
	mFramesCount--;
}

StackRef<Method> TaskContext::getMethod()
{
	return StackRef<Method>(getFrame(), Frame::INDEX_METHOD);
}