#include "stdafx.h"
#include "FrameInspector.h"
#include "Frame.h"

#include "Thread.h"
#include "Class.h"
#include "String.h"

#include "ObjectInspector.h"

using namespace Beer;


FrameInspector::FrameInspector()
	: mFrame(NULL), mFrameStackNext(0), mFramePtr(NULL)
{
}

FrameInspector::~FrameInspector()
{
}

void FrameInspector::start(Frame* frame)
{
	start(frame, NULL);
}

void FrameInspector::start(Frame* frame, Frame** framePtr)
{
	mFrame = frame;
	mFrameStackNext = 0;
	mLevel = 0;
	mFramePtr = framePtr;
}

void FrameInspector::nextFrame()
{
	if(mFrame->isStackAllocated())
	{
		start(mFrame - 1);
	}
	else
	{
		start(*reinterpret_cast<Frame**>(mFrame->bp()), reinterpret_cast<Frame**>(mFrame->bp()));
	}
}

void FrameInspector::nextInnerFrame()
{
	if(mFrame->hasInnerFrame())
	{
		mFrame = mFrame->getInnerFrame();
	}
	else
	{
		mFrame = NULL;
	}
}

bool FrameInspector::hasFrame()
{
	return mFrame != NULL;
}

Frame* FrameInspector::getFrame()
{
	return mFrame;
}

Frame** FrameInspector::getFramePtr()
{
	return mFramePtr;
}

void FrameInspector::nextObject()
{
	mFrameStackNext++;
}

bool FrameInspector::hasObject()
{
	return mFrameStackNext < mFrame->stackLength();
}

Object* FrameInspector::getObject()
{
	return mFrame->stackTop(mFrame->translateAbsolute(mFrameStackNext));
}

Object** FrameInspector::getObjectPtr()
{
	return mFrame->stackTopPtr(mFrame->translateAbsolute(mFrameStackNext));
}

StackRef<Object> FrameInspector::getObjectRef()
{
	return StackRef<Object>(mFrame, mFrame->translateAbsolute(mFrameStackNext));
}

void FrameInspector::traverseObjectPtr(Object** ptrToObject)
{
	printPadding();
	cout << "- ";

	cout << "**traverseObjectPtr**";
}

void FrameInspector::printPadding()
{
	for(int32 i = 0; i < mLevel; i++)
	{
		cout << " ";
	}
}

void FrameInspector::debugPrintFrames(Thread* thread)
{
	cout << "--------------------Frames traversing---------------------\n";
	Frame* frame = thread->getFrame();

	FrameInspector frameInspector;
	frameInspector.start(frame);

	while(frameInspector.hasFrame())
	{
		frame = frameInspector.getFrame();
		cout << "// #" << frame << " alloc:" << (frame->isStackAllocated() ? "stack" : "heap");
		cout << ", pc:" << frame->getProgramCounter();
		cout << ", offset:" << frame->getFrameOffset();
		cout << ", stack:" << frame->stackLength();
		cout << "\n";

		if(!frameInspector.hasObject())
		{
			cout << "- empty\n";
		}

		while(frameInspector.hasObject())
		{
			Object* object = frameInspector.getObject();

			ObjectInspector objectInspector(frameInspector.mLevel + 1);
			objectInspector.start(thread, object);
			objectInspector.debugPrintObject();

			cout << BEER_WIDEN("\n");
			frameInspector.nextObject();
		}

		frameInspector.nextFrame();
		if(frameInspector.hasFrame())
		{
			cout << "-----------\n";
		}
	}

	cout << "-----------------------------------------------------------\n\n";
}