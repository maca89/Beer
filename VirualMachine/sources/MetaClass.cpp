#include "stdafx.h"
#include "MetaClass.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL MetaClass::init(VirtualMachine* vm, StackFrame* frame, StackRef<MetaClass> receiver, StackRef<MetaClass> ret)
{
	ret = receiver;
}

void BEER_CALL MetaClass::findMethod(VirtualMachine* vm, StackFrame* frame, StackRef<MetaClass> receiver, StackRef<String> selector, StackRef<Method> ret)
{
	// TODO
}