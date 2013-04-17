#include "stdafx.h"
#include "Boolean.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "ClassLoader.h"

using namespace Beer;

Boolean* Boolean::False = Boolean::makeInlineValue(false);
Boolean* Boolean::True = Boolean::makeInlineValue(true);

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

Class* BooleanClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, 0, 7 + Object::OBJECT_METHODS_COUNT);
}

void BooleanClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->setSuperClass(thread->getVM()->getObjectClass());
	klass->markAsValueType();
	klass->markSealed();
	
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("Boolean"), BEER_WIDEN("Boolean::Boolean()"), &Boolean::init, 1, 0);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("!"), BEER_WIDEN("Boolean::!()"), &Boolean::operatorNegation, 1, 0);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("=="), BEER_WIDEN("Boolean::==(Boolean)"), &Boolean::operatorEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("!="), BEER_WIDEN("Boolean::!=(Boolean)"), &Boolean::operatorNotEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("&&"), BEER_WIDEN("Boolean::&&(Boolean)"), &Boolean::operatorAnd, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("||"), BEER_WIDEN("Boolean::||(Boolean)"), &Boolean::operatorOr, 1, 1);

	loader->addOverrideMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Boolean::createInstance, 1, 0);
}
