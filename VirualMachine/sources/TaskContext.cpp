#include "stdafx.h"
#include "TaskContext.h"
#include "StackRef.h"
#include "Method.h"
#include "Frame.h"
#include "ThreadSafeOutput.h"

using namespace Beer;

//#define BEER_TASKCONTEXT_VERBOSE

#ifdef BEER_TASKCONTEXT_VERBOSE
#define TASKCONTEXT_DEBUG(msg) { stringstream ss; ss << "TaskContext: " << msg << "\n"; ThreadSafeOutput(cout) << ss.str(); }
#else
#define TASKCONTEXT_DEBUG(msg)
#endif // BEER_SCHEDULER_VERBOSE


TaskContext::TaskContext()
	: mHeap(NULL), /*mRootFrame(NULL),*/ mTopFrame(NULL), mFramesCount(0), mFrameClass(NULL)
{
	//cout << "TaskContext " << this << " allocated on heap\n";
}

TaskContext::~TaskContext()
{
}

void TaskContext::init(Heap* heap, Class* frameClass)
{
	//mRootFrame = NULL;
	mTopFrame = NULL;
	mHeap = heap;
	mFrameClass = frameClass;
	// just a temporary solution, TODO
	//mRootFrame = allocFrame(NULL, 250, 0);
	mTopFrame = allocFrame(NULL, 30, 2);
	//frame->setFrameOffset(2);
	mTopFrame->stackPush(); // simulate receiver
	mTopFrame->stackPush(); // simulate method
	//mRootFrame->stackPush(frame);
	//fetchTopFrame();
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
		frame->setType(mFrameClass); // TODO
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

	mTopFrame = newFrame;
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
	mTopFrame = previousFrame;
	mFramesCount--;
}

StackRef<Method> TaskContext::getMethod()
{
	return StackRef<Method>(getFrame(), Frame::INDEX_METHOD);
}

void TaskContext::updateFramesClass(Class* klass)
{
	mFrameClass = klass;
	
	Frame* frame = mTopFrame;

	while(frame)
	{
		if(frame->isHeapAllocated())
		{
			frame->setType(klass);
		}
		
		frame = frame->previousFrame();
	}
}

void TaskContext::updateMovedPointers(GenerationalGC* gc)
{
	if(mTopFrame)
	{

		mTopFrame = updateFramePointers(gc, mTopFrame);

		/*Frame* frame = mTopFrame;

		bool first = true;

		while(frame)
		{
			Frame* tmp = frame;
			frame = updateFramePointers(gc, frame);
			//cout << "updated frame from #" << tmp << " to #" << frame << "\n";

			if(first)
			{
				//cout << "updated top frame from #" << mTopFrame << " to #" << frame << "\n";
				mTopFrame = frame;
				first = false;
			}

			if(frame->isStackAllocated())
			{
				// go through all the stack allocated, which were already updated
				while(frame->isStackAllocated())
				{
					frame = frame->previousFrame();
				}

				// pass the first heap allocated, which was already updated
				frame = frame->previousFrame();
			}
		}*/
	}
}

// very ugly, too much resursive, TODO
Frame* TaskContext::updateFramePointers(GenerationalGC* gc, Frame* frame)
{
	Frame* frameUpdated = frame;
	TASKCONTEXT_DEBUG("Updating frame #" << frame << " " << (frame->isStackAllocated() ? "[stack allocated]" : "[heap allocated]"));

	if(frame->isStackAllocated())
	{
		Frame* prevFrame = frame->previousFrame();
		Frame* prevFrameUpdated = updateFramePointers(gc, prevFrame);
		//cout << "updated frame from #" << prevFrame << " to #" << prevFrameUpdated << "\n";
		
		frameUpdated = reinterpret_cast<Frame*>(prevFrameUpdated->getNewFrameStart());

		//cout << "updated bp from #" << frame->bp() << " to #" << prevFrameUpdated->sp() << "\n";
		frameUpdated->bp(prevFrameUpdated->sp());
	}
	else // heap allocated
	{
		frameUpdated = static_cast<Frame*>(gc, gc->getIdentity(frame));
		TASKCONTEXT_DEBUG("Was moved to   #" << frameUpdated);

		int64 offset = (reinterpret_cast<byte*>(frameUpdated->bp()) - reinterpret_cast<byte*>(frame));
		void* newBp = reinterpret_cast<byte*>(frameUpdated) + offset;	
		
		TASKCONTEXT_DEBUG("bp changed from #" << frameUpdated->bp() << " to #" << newBp);
		frameUpdated->bp(newBp);

		Frame* prevFrame = frameUpdated->previousFrame();
		
		if(prevFrame)
		{
			TASKCONTEXT_DEBUG("Has previous frame #" << prevFrame);
			Frame* prevFrameUpdated = updateFramePointers(gc, prevFrame);
			TASKCONTEXT_DEBUG("(*bp) changed from #" << (*reinterpret_cast<void**>(frameUpdated->bp())) << " to #" << prevFrameUpdated);
			*reinterpret_cast<Frame**>(frameUpdated->bp()) = prevFrameUpdated; // maybe OK
		}
		else
		{
			TASKCONTEXT_DEBUG("Has no previous frame");
			//*reinterpret_cast<Frame**>(newBp) = NULL; // maybe OK
		}
	}

	return frameUpdated;
}