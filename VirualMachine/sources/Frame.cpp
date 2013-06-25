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
			Object* object = insp.getObject();
			receiver->traverseObjectPtr(&object); // !!! should not pass real Object** because pointer updates after safe point is not ready for this !!!
			insp.nextObject();
		}

		insp.nextInnerFrame();
	}
}