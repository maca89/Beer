#include "stdafx.h"
#include "Boolean.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "ClassLoader.h"

using namespace Beer;


void BEER_CALL Boolean::init(Thread* thread, StackRef<Boolean> receiver, StackRef<Boolean> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Boolean::operatorNegation(Thread* thread, StackRef<Boolean> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(!receiver->getData());
}

void BEER_CALL Boolean::operatorEqual(Thread* thread, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Boolean::operatorNotEqual(Thread* thread, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
}

void BEER_CALL Boolean::operatorAnd(Thread* thread, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() && arg->getData());
}

void BEER_CALL Boolean::operatorOr(Thread* thread, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() || arg->getData());
}

void BEER_CALL Boolean::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(false);
}

Class* BooleanClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 7);
}

void BooleanClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	loader->extendClass(klass, vm->getObjectClass());
	
	loader->addMethod(klass, BEER_WIDEN("Boolean"), BEER_WIDEN("Boolean::Boolean()"), &Boolean::init, 1, 0);

	loader->addMethod(klass, BEER_WIDEN("!"), BEER_WIDEN("Boolean::!()"), &Boolean::operatorNegation, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("=="), BEER_WIDEN("Boolean::==()"), &Boolean::operatorEqual, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("!="), BEER_WIDEN("Boolean::!=()"), &Boolean::operatorNotEqual, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("&&"), BEER_WIDEN("Boolean::&&()"), &Boolean::operatorAnd, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("||"), BEER_WIDEN("Boolean::||()"), &Boolean::operatorOr, 1, 1);

	loader->addMethod(klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Boolean::createInstance, 1, 0);
}
