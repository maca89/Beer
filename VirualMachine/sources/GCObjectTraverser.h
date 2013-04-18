#pragma once

#include "TraverseObjectReceiver.h"
#include <stack>
#include "TaskScheduler.h"
#include "NotifyHeap.h"
#include "ChildrenForwarder.h"

namespace Beer
{	
	class Object;
	class GCObject;
	class GenerationalGC;

	class GCObjectTraverser : protected TraverseObjectReceiver
	{
	protected:

		typedef GenerationalGC GC;
		typedef std::stack<Object*> ObjectStack;

	protected:

		NotifyHeap* mNursery;
		Heap* mMature;
		ObjectStack mGrey;
		ChildrenForwarder mForwarder;

	public:

		INLINE GCObjectTraverser(NotifyHeap* nursery, Heap* mature)
			:	mNursery(nursery),
				mMature(mature)
		{ }

		void traverseObjects(TaskScheduler* scheduler);

	protected:

		void traverseTaskQueue(TaskScheduler::TaskQueue* queue);
		void markTaskRoots(Frame* frame);
		void traverseGrey();

		void trace(Object* obj);
		void traceFrame(Frame* frame);
		void forwardFrameChildren(Frame* frame);

		INLINE bool isTraced(Object* obj)
		{
			return obj != NULL	&& // pointer is not NULL
				!Object::isInlineValue(obj) && // pointer is not inlined value
				mNursery->contains(reinterpret_cast<byte*>(obj)); // pointer is within collected heap
		}

		virtual void traverseObjectPtr(Object** ptrToObject);
	};
};