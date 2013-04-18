#include "stdafx.h"
#include "GCObjectTraverser.h"

#include "FrameInspector.h"

using namespace Beer;

void GCObjectTraverser::traverseObjects(TaskScheduler* scheduler)
{
	traverseTaskQueue(scheduler->getActiveQueue());
	//traverseTaskQueue(scheduler->getDoneQueue());
	//traverseTaskQueue(scheduler->getScheduledQueue());
	//traverseTaskQueue(scheduler->getWaitingQueue());

	traverseGrey();
}

void GCObjectTraverser::traverseTaskQueue(TaskScheduler::TaskQueue* queue)
{
	for (TaskScheduler::TaskQueue::iterator it = queue->begin(); it != queue->end(); it++)
	{
		markTaskRoots((*it)->getContext()->getFrame());
	}
}

void GCObjectTraverser::markTaskRoots(Frame* frame)
{
	/*if (isTraced(frame))
	{
	mGrey.push(frame);
	GCObject::get(frame)->setFlag(GCObject::GC_FLAG_GREY);
	}*/

	FrameInspector ins;
	ins.start(frame);

	while (ins.hasFrame())
	{
		Frame* f = ins.getFrame();

		if (!f->isStackAllocated() && isTraced(f))
		{
			mGrey.push(f);
		}

		/*		while (ins.hasObject())
		{

		Object* obj = ins.getObject();

		if (isTraced(obj))
		{
		mGrey.push(obj);
		}

		ins.nextObject();
		}*/

		ins.nextFrame();
	}
}

void GCObjectTraverser::traverseGrey()
{
	while (!mGrey.empty())
	{
		trace(mGrey.top());
	}
}

void GCObjectTraverser::traverseObjectPtr(Object** ptrToObject)
{
	Object* obj = *ptrToObject;

	if (isTraced(obj))
	{
		mGrey.push(obj);
	}
}

void GCObjectTraverser::trace(Object* obj)
{
	//DBG_ASSERT(obj->getType() != NULL, BEER_WIDEN("Type of object is NULL"));

	GCObject* gcObj = GCObject::get(obj);

	if (gcObj->hasFlag(GCObject::GC_FLAG_WHITE))
	{
		gcObj->setFlag(GCObject::GC_FLAG_GREY);

		GCObject* newGCObj = reinterpret_cast<GCObject*>(mMature->alloc(gcObj->getSize()));

		::memcpy(newGCObj, gcObj, gcObj->getSize());

		newGCObj->setFlag(GCObject::GC_FLAG_WHITE);

		gcObj->setPtr(newGCObj->getObject());

		if (obj->getType())
		{
			obj->getType()->getTraverser()(this, obj->getType(), obj);
		}
		else
		{
			traceFrame(static_cast<Frame*>(obj));
		}
	}
	else
	{
		if (obj->getType())
		{
			mForwarder.forward(gcObj);
		}
		else
		{
			forwardFrameChildren(static_cast<Frame*>(obj));
		}

		gcObj->setFlag(GCObject::GC_FLAG_BLACK);

		mGrey.pop();
	}
}

void GCObjectTraverser::traceFrame(Frame* frame)
{
	FrameInspector ins;
	ins.start(frame);

	while (ins.hasObject())
	{
		Object* obj = ins.getObject();

		if (isTraced(obj))
		{
			mGrey.push(obj);
		}

		ins.nextObject();
	}
}

void GCObjectTraverser::forwardFrameChildren(Frame* frame)
{
	FrameInspector ins;
	ins.start(frame);

	while (ins.hasObject())
	{
		Object** ptrToObj = ins.getObjectPtr();
		Object* obj = *ptrToObj;

		if (obj != NULL && !Object::isInlineValue(obj)) // Checking if pointer points to nursery heap is not necessary, but it might be faster than forwarding every pointer
		{
			*ptrToObj = GCObject::get(obj)->forward();
		}		

		ins.nextObject();
	}
}