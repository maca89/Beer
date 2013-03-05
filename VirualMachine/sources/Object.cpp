#include "stdafx.h"
#include "Object.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL Object::init(Thread* thread/*, StackFrame* frame*/, StackRef<Object> receiver, StackRef<Object> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Object::setClass(Thread* thread/*, StackFrame* frame*/, StackRef<Object> receiver, StackRef<Object> param)
{
	receiver->setChild(1, param.get()); // TODO
}

void BEER_CALL Object::getClass(Thread* thread/*, StackFrame* frame*/, StackRef<Object> receiver, StackRef<Object> ret)
{
	ret = receiver->getChild(1); // TODO
}



/*ClassReflection* ObjectClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<ObjectClass>(name, 0, 0, 0);
}*/

/*void ObjectClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	//MethodReflection* initMethod = loader->createMethod<MethodReflection>(BEER_WIDEN("Object"), 1, 0);
	//initMethod->setFunction(&BeerObject_init);
	//klass->setMethod(0, vm->createPair(vm->createString((string(klass->getName()) + BEER_WIDEN("::Object()")).c_str()), initMethod));
}*/