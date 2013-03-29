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
		uint32 mFrameStackNext;
		int32 mLevel;

	public:
		FrameInspector();
		~FrameInspector();

		void start(Frame* frame);

		void nextObject();
		bool hasObject();
		Object* getObject();
		Object** getObjectPtr();
		StackRef<Object> getObjectRef();

		void nextFrame();
		bool hasFrame();
		Frame* getFrame();

		static void debugPrintFrame(Thread* thread, Frame* frame);
		static void debugPrintFrames(Thread* thread);

		// interface TraverseObjectReceiver
		virtual void traverseObjectPtr(Object** ptrToObject);

	protected:
		void printPadding();
	};
};