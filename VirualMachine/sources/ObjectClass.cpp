#include "stdafx.h"
#include "ObjectClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL BeerObject_init(VirtualMachine* vm, StackFrame* frame, StackRef<Object> receiver, StackRef<Object> ret1)
{
	ret1 = receiver;
}

ClassReflection* ObjectClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<ObjectClass>(name, 0, 0, 1);
}

void ObjectClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	MethodReflection* initMethod = loader->createMethod<MethodReflection>("Object", std::string(klass->getName()) + "::Object()", 1, 0);
	initMethod->setFunction(&BeerObject_init);
	klass->setMethod(0, initMethod);
}