#include "stdafx.h"
#include "TrampolineThread.h"
#include "Debugger.h"
#include "ConsoleClass.h"
#include "MethodReflection.h"

using namespace Beer;


void TrampolineThread::work()
{
	mVM->getDebugger()->started();
		
	while(hasStackFrame())
	{
		StackFrame* frame = getStackFrame();
		
		// return
		if(frame->method == NULL)
		{
			closeStackFrame();
			continue;
		}

		try
		{
		#ifdef BEER_DEBUG_MODE
			if(mVM->getDebugger()->isEnabled()) mVM->getDebugger()->step(&mFrames, frame);
		#endif // BEER_DEBUG_MODE

		#ifdef BEER_DEBUG_MODE
			mVM->getHeap()->collect();
		#endif // BEER_DEBUG_MODE

			MethodReflection* method = frame->method->call(mVM, &mFrames, frame);
			
			if(frame->done)
			{
				closeStackFrame();
			}
			
			if(method)
			{
				openStackFrame()->method = method;
			}

		#ifdef BEER_DEBUG_MODE
			Console::getOutput().flush(cout);
		#endif // BEER_DEBUG_MODE
		}
		catch(Exception& ex)
		{
			if(!mVM->getDebugger()->catchException(&mFrames, frame, ex))
			{
				break;//throw ex;
			}
		}
	}

	mVM->getDebugger()->ended();

	//Console::getOutput() << BEER_WIDEN("Exiting Thread\n");

	// flush all output
	Console::getOutput().flush(cout);
}