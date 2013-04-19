#pragma once
#include "prereq.h"
#include "Frame.h"
#include "StackRef.h"
//#include "Method.h"


namespace Beer
{
	class Frame;
	class Heap;
	class GenerationalGC;

	class TaskContext
	{
	public:
		enum
		{
			DEFAULT_FRAME_SPACE = 4*1024, // 4KB
		};

	protected:
		Heap* mHeap;
		Frame* mTopFrame;
		int64 mFramesCount;
		Class* mFrameClass;

	public:
		TaskContext();
		~TaskContext();

		void init(Heap* heap, Class* frameClass);
		void updateMovedPointers(GenerationalGC* gc);
		
		INLINE Frame* getFrame() { DBG_ASSERT(mTopFrame != NULL, BEER_WIDEN("No stack frame")); return mTopFrame; }
		INLINE bool hasFrame() { return mTopFrame != NULL; }
		Frame* openFrame();
		void closeFrame();
		Frame* getPreviousFrame();
		void updateFramesClass(Class* klass);

		// TODO: get rid of these
		INLINE void setHeap(Heap* value) { mHeap = value; }

		// 
		StackRef<Method> getMethod();

	protected:
		Frame* allocFrame(Frame* previousFrame, uint32 stackSize, uint32 argsCout);
		void discardFrame(Frame* previousFrame, Frame* currentFrame);

		Frame* updateFramePointers(GenerationalGC* gc, Frame* frame);
	};

	INLINE Frame* TaskContext::getPreviousFrame()
	{
		if(hasFrame())
		{
			return getFrame()->previousFrame();
		}

		return NULL;
	}

};