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
#ifdef BEER_INLINE_FRAMES
			DEFAULT_FRAME_SPACE = 4*1024, // 4KB
#else
			DEFAULT_FRAME_SPACE = 0,
#endif // BEER_INLINE_FRAMES
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
		
		INLINE Frame* getFrame() { return mTopFrame; }
		INLINE Frame** getFramePtr() { return &mTopFrame; }
		INLINE bool hasFrame() { return mTopFrame != NULL; }
		Frame* openFrame();
		void closeFrame();
		Frame* getPreviousFrame();
		void updateFramesClass(Class* klass);
		int64 getFramesCount() const;

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

	INLINE int64 TaskContext::getFramesCount() const
	{
		return mFramesCount;
	}

};