#include "stdafx.h"
#include "prereq.h"
#include "StringClass.h"
#include "VirtualMachine.h"
#include "Object.h"
#include "ClassReflection.h"

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

#undef MethodNotFoundException

MethodNotFoundException::MethodNotFoundException(Object* instance, ClassReflection* klass, String* selector, string filename, long line)
	: RuntimeException(
		string(BEER_WIDEN("No method ")) + selector->c_str() + BEER_WIDEN(" for ") + klass->getName(), 
		filename, 
		line
	)
{
	mName = BEER_WIDEN("MethodNotFoundException");
}