#pragma once
#include "prereq.h"
#include "Frame.h"
#include "StackRef.h"
//#include "Method.h"


namespace Beer
{
	class Frame;
	class Heap;

	class TaskContext
	{
	public:
		enum
		{
			DEFAULT_FRAME_SPACE = 4*1024, // 4KB
		};

	protected:
		Heap* mHeap;
		Frame* mRootFrame;
		Frame* mTopFrame;
		int64 mFramesCount;

	public:
		TaskContext();
		~TaskContext();

		void init(Heap* heap);

		INLINE Frame* getFrame() { DBG_ASSERT(mTopFrame != NULL, BEER_WIDEN("No stack frame")); return mTopFrame; }
		INLINE bool hasFrame() { return mRootFrame->stackLength() != 0; }
		Frame* openFrame();
		void closeFrame();
		Frame* getPreviousFrame();

		// TODO: get rid of these
		INLINE Frame* getFrames() { return mRootFrame; }
		INLINE void setHeap(Heap* value) { mHeap = value; }

		// 
		StackRef<Method> getMethod();

	protected:
		INLINE void fetchTopFrame()
		{
			if(hasFrame()) mTopFrame = mRootFrame->stackTop<Frame>();
			else mTopFrame = NULL;
		}

		Frame* allocFrame(Frame* previousFrame, uint32 stackSize, uint32 argsCout);
		void discardFrame(Frame* previousFrame, Frame* currentFrame);
	};
};