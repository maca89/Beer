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
		MethodReflection* method = frame->method;
		
		// return
		if(method == NULL)
		{
			closeStackFrame();
			continue;
		}

		try
		{
		#ifdef BEER_DEBUG_MODE
			if(mVM->getDebugger()->isEnabled()) mVM->getDebugger()->step(frame);
		#endif // BEER_DEBUG_MODE

		//#ifdef BEER_DEBUG_MODE
			Console::getOutput().flush(cout);
		//#endif // BEER_DEBUG_MODE

		#ifdef BEER_DEBUG_MODE
			//mVM->getHeap()->collect(); // TODO
		#endif // BEER_DEBUG_MODE

		#ifdef BEER_MEASURE_PERFORMANCE
			MiliTimer timer;
			timer.start();
			MethodReflection* oldMethod = method;
		#endif // BEER_MEASURE_PERFORMANCE

			method = method->call(mVM, frame);

		#ifdef BEER_MEASURE_PERFORMANCE
			oldMethod->addTimeSpent(timer.stop());
		#endif // BEER_MEASURE_PERFORMANCE
			
			if(method)
			{
				StackFrame* newFrame = openStackFrame();
				newFrame->method = method;
				newFrame->stack->check(method->getMaxStack());
			}
			else
			{
				closeStackFrame();
			}
		}
		catch(Exception& ex)
		{
			if(!mVM->getDebugger()->catchException(frame, ex))
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