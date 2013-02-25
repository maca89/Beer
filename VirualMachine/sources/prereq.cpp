#include "stdafx.h"
#include "prereq.h"
#include "StringClass.h"
#include "VirtualMachine.h"
#include "Object.h"
#include "Class.h"
#include "Method.h"

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
		string(BEER_WIDEN("Method ")) + method->getName()->c_str() + BEER_WIDEN(" is abstract"), // TODO
		filename, 
		line
	)
{
	mName = BEER_WIDEN("AbstractMethodException");
}


#undef MethodNotFoundException

MethodNotFoundException::MethodNotFoundException(Object* instance, Class* klass, String* selector, string filename, long line)
	: RuntimeException(
		string(BEER_WIDEN("No method ")) + selector->c_str() + BEER_WIDEN(" for ") + klass->getName()->c_str(), // TODO
		filename, 
		line
	)
{
	mName = BEER_WIDEN("MethodNotFoundException");
}