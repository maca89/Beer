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

		/*if(frame == (Frame*)0xCCCCCCCC)
		{
			int a = 0;
		}*/

		while(insp.hasObject())
		{
			/*Object* object = ;

			if(object == (Object*)0xCCCCCCCC)
			{
				int a = 0;
			}


			cout << ""*/
			// not sure if getObjectPtr is right choice
			receiver->traverseObjectPtr(insp.getObjectPtr()); // !!! should not pass real Object** because pointer updates after safe point is not ready for this !!!
			insp.nextObject();
		}

		insp.nextInnerFrame();
	}
}