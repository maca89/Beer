#pragma once
#include "prereq.h"
#include "Frame.h"
#include "StackRef.h"
#include "Class.h"


namespace Beer
{
	class FrameInspector : public TraverseObjectReceiver
	{
	public:

	protected:
		Frame* mFrame;
		Frame** mFramePtr;
		uint32 mFrameStackNext;
		int32 mLevel;

	public:
		FrameInspector();
		~FrameInspector();

		void start(Frame* frame);
		void start(Frame* frame, Frame** framePtr);

		void nextObject();
		bool hasObject();
		Object* getObject();
		Object** getObjectPtr();
		StackRef<Object> getObjectRef();

		void nextFrame();
		void nextInnerFrame();
		bool hasFrame();
		Frame* getFrame();
		Frame** getFramePtr();

		static void debugPrintFrame(Thread* thread, Frame* frame);
		static void debugPrintFrames(Thread* thread);

		// interface TraverseObjectReceiver
		virtual void traverseObjectPtr(Object** ptrToObject);

	protected:
		void printPadding();
	};
};