#include "stdafx.h"
#include "MetaClass.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL MetaClass::init(Thread* thread/*, StackFrame* frame*/, StackRef<MetaClass> receiver, StackRef<MetaClass> ret)
{
	ret = receiver;
}

void BEER_CALL MetaClass::findMethod(Thread* thread/*, StackFrame* frame*/, StackRef<MetaClass> receiver, StackRef<String> selector, StackRef<Method> ret)
{
	// TODO
}