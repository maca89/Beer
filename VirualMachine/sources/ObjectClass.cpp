#include "stdafx.h"
#include "ObjectClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


/*void BEER_CALL BeerObject_init(VirtualMachine* vm, StackFrame* frame, StackRef<Object> receiver, StackRef<Object> ret1)
{
	ret1 = receiver;
}*/

/*ClassReflection* ObjectClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<ObjectClass>(name, 0, 0, 0);
}

void ObjectClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	//MethodReflection* initMethod = loader->createMethod<MethodReflection>(BEER_WIDEN("Object"), 1, 0);
	//initMethod->setFunction(&BeerObject_init);
	//klass->setMethod(0, vm->createPair(vm->createString((string(klass->getName()) + BEER_WIDEN("::Object()")).c_str()), initMethod));
}*/