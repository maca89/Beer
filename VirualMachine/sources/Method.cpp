#include "stdafx.h"
#include "Method.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "StackFrame.h"

using namespace Beer;


Method* Method::runFunction(VirtualMachine* vm, StackFrame* frame)
{
	Cb fn = mFunction;
	if(fn == NULL)
	{
		// TODO
		throw AbstractMethodException(this);
	}

	//NULL_ASSERT(fn);

	uint16 i = 0;
	int32 elemi = 0;
	uint16 returnCount = getReturnsCount();
	uint16 paramsCount = getParamsCount();

	uint32 pushedSize
		= sizeof(StackRef<Object>) * returnCount						// returns
		+ sizeof(StackRef<Object>) * paramsCount						// params
		+ sizeof(StackRef<Object>)										// receiver
		+ sizeof(StackFrame*)											// stackframe
		+ sizeof(VirtualMachine*);										// vm
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////// within the next block you should not do anything with the stack 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(i = 0; i < returnCount; i++)
	{
		elemi = -(static_cast<int32>(returnCount) - static_cast<int32>(i)) - static_cast<int32>(paramsCount);

		__asm // push return
		{
			push elemi;
			push frame;
		}
	}
	for(i = 0; i < paramsCount; i++)
	{
		elemi = -(static_cast<int32>(paramsCount) - static_cast<int32>(i));

		__asm // push param
		{
			push elemi;
			push frame;
		}
	}
	__asm
	{
		// push receiver
		push 0;
		push frame;

		// push StackFrame*
		push frame;

		// push VirtualMachine*
		push vm;

		// call function
		call fn;

		// fix stack
		add esp, pushedSize;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	frame->stackMoveTop(-(paramsCount + 1));

	return NULL; // TODO
}
