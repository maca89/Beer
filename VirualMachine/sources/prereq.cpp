#include "stdafx.h"
#include "prereq.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Object.h"
#include "Class.h"
#include "Method.h"
#include "Frame.h"

using namespace Beer;


/*void ZeroMemory(void* mem, size_t size)
{
	//  TODO: does it really works?
	while(size > 0)
	{
		*reinterpret_cast<char_t*>(mem) = 0;
		mem = reinterpret_cast<char_t*>(mem) + 1;
		size--;
	}
}*/

#undef AbstractMethodException

AbstractMethodException::AbstractMethodException(Method* method, string filename, long line)
	: RuntimeException(
	string(BEER_WIDEN("Method ")) + ((String*)method->getChildren()[Method::CHILD_ID_METHOD_NAME])->c_str() + BEER_WIDEN(" is abstract"), // TODO
		filename, 
		line
	)
{
	mName = BEER_WIDEN("AbstractMethodException");
}


#undef MethodNotFoundException

MethodNotFoundException::MethodNotFoundException(Object* instance, Class* klass, String* selector, string filename, long line)
	: RuntimeException(
		string(BEER_WIDEN("No method ")) + selector->c_str() + BEER_WIDEN(" for ") + ((String*)klass->getChildren()[Class::CHILD_ID_CLASS_NAME])->c_str(), // TODO
		filename, 
		line
	)
{
	mName = BEER_WIDEN("MethodNotFoundException");
}


#if defined(BEER_STACK_DEBUGGING)
DebugStackCheck::DebugStackCheck(Frame* frame) : frame(frame)
{
	startIndex = frame->stackSize();
}

DebugStackCheck::~DebugStackCheck()
{
	if(frame->stackSize() != startIndex)
	{
		BEER_DBG_BREAKPOINT();
		throw CriticalAssertException(BEER_WIDEN("Stack is corrupted"));
	}
}
#endif // BEER_STACK_DEBUGGING