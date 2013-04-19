#include "stdafx.h"
#include "Frame.h"
#include "TraverseObjectReceiver.h"
#include "Class.h"
#include "FrameInspector.h"

using namespace Beer;


void Frame::FrameTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* inst)
{
	Class::DefaultInstanceTraverser(receiver, klass, inst);

	Frame* instance = static_cast<Frame*>(inst);
	
	FrameInspector insp;
	insp.start(instance);

	while(insp.hasFrame())
	{
		Frame* frame = insp.getFrame();

		while(insp.hasObject())
		{
			receiver->traverseObjectPtr(insp.getObjectPtr());
			insp.nextObject();
		}

		insp.nextInnerFrame();
	}
}