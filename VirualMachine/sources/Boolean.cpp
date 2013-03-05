#include "stdafx.h"
#include "Boolean.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "ClassLoader.h"

using namespace Beer;


void BEER_CALL Boolean::init(Thread* thread/*, StackFrame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Boolean::operatorNegation(Thread* thread/*, StackFrame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(!receiver->getData());
}

void BEER_CALL Boolean::operatorEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Boolean::operatorNotEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
}

void BEER_CALL Boolean::operatorAnd(Thread* thread/*, StackFrame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() && arg->getData());
}

void BEER_CALL Boolean::operatorOr(Thread* thread/*, StackFrame* frame*/, StackRef<Boolean> receiver, StackRef<Boolean> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() || arg->getData());
}

void BEER_CALL Boolean::createInstance(Thread* thread/*, StackFrame* frame*/, StackRef<Class> receiver, StackRef<Boolean> ret)
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
	klass->extends(0, vm->getObjectClass());
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Boolean")));
	method->setFunction(&Boolean::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::Boolean()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("!")));
	method->setFunction(&Boolean::operatorNegation);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::!()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("==")));
	method->setFunction(&Boolean::operatorEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::==(Boolean)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("&&")));
	method->setFunction(&Boolean::operatorAnd);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::&&(Boolean)")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("||")));
	method->setFunction(&Boolean::operatorOr);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::||(Boolean)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&Boolean::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
}
