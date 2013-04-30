#include "stdafx.h"
#include "TrampolineThread.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "Console.h"
#include "Method.h"

using namespace Beer;


void TrampolineThread::work()
{
	trampoline();
}


bool TrampolineThread::trampoline()
{
	//mVM->getDebugger()->started();

	bool hf = hasFrame();
	bool iep = isExecutionPaused();
		
	while(hasFrame() && !isExecutionPaused())
	{
		Frame* frame = getFrame();
		StackRef<Method> method(frame, Frame::INDEX_METHOD);
		
		if(method.isNull())
		{
			closeFrame();
			continue;
		}

		try
		{
		#ifdef BEER_DEBUG_MODE
			if(mVM->getDebugger()->isEnabled()) mVM->getDebugger()->step(this, frame);
		#endif // BEER_DEBUG_MODE

		#ifdef BEER_MEASURE_PERFORMANCE
			MiliTimer timer;
			timer.start();
			MethodReflection* oldMethod = method;
		#endif // BEER_MEASURE_PERFORMANCE

			method->invoke(this);

		#ifdef BEER_MEASURE_PERFORMANCE
			oldMethod->addTimeSpent(timer.stop());
		#endif // BEER_MEASURE_PERFORMANCE
		}
		catch(Exception& ex)
		{
			if(!mVM->getDebugger()->catchException(this, frame, ex))
			{
				break;//throw ex;
			}
		}
	}

	//mVM->getDebugger()->ended();
	return !hasFrame();
}