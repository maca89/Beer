#include "stdafx.h"
#include "Integer.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Param.h"
#include "Float.h"
#include "Boolean.h"

using namespace Beer;


void BEER_CALL Integer::init(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Integer::operatorFloat(Thread* thread, StackRef<Integer> receiver, StackRef<Float> ret)
{
	thread->createFloat(ret, static_cast<Float::FloatData>(receiver->getData()));
}

void BEER_CALL Integer::operatorString(Thread* thread, StackRef<Integer> receiver, StackRef<String> ret)
{
	stringstream ss;
	ss << receiver->getData();
	// TODO: check and throw Exception
	thread->createString(ret, ss.str());
}

void BEER_CALL Integer::operatorAdd(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() + arg->getData());
}

void BEER_CALL Integer::operatorSub(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() - arg->getData());
}

void BEER_CALL Integer::operatorMul(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() * arg->getData());
}

void BEER_CALL Integer::operatorMod(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() % arg->getData());
}

void BEER_CALL Integer::operatorDiv(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Float> ret)
{
	thread->createFloat(ret, ((Float::FloatData)receiver->getData()) / arg->getData());
}

void BEER_CALL Integer::operatorMinus(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, -receiver->getData());
}

void BEER_CALL Integer::operatorEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Integer::operatorNotEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
}

void BEER_CALL Integer::operatorSmaller(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() < arg->getData());
}

void BEER_CALL Integer::operatorSmallerEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() <= arg->getData());
}

void BEER_CALL Integer::operatorGreater(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() > arg->getData());
}

void BEER_CALL Integer::operatorGreaterEqual(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() >= arg->getData());
}

void BEER_CALL Integer::abs(Thread* thread, StackRef<Integer> receiver, StackRef<Integer> ret)
{
	Integer::IntegerData value = receiver->getData();
	if(value < 0)
	{
		thread->createInteger(ret, -value);
	}
	else
	{
		ret = receiver;
	}
}

void BEER_CALL Integer::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	ret = Integer::makeInlineValue(0);
}

Class* IntegerClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, 0, 17 + Object::OBJECT_METHODS_COUNT);
}

void IntegerClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->setSuperClass(thread->getVM()->getObjectClass());
	klass->markSealed();
	klass->markAsValueType();

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("Integer"), BEER_WIDEN("Integer::Integer()"), &Integer::init, 1, 0);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("Float"), BEER_WIDEN("Integer::Float()"), &Integer::operatorFloat, 1, 0);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("+"), BEER_WIDEN("Integer::+(Integer)"), &Integer::operatorAdd, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("-"), BEER_WIDEN("Integer::-(Integer)"), &Integer::operatorSub, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("*"), BEER_WIDEN("Integer::*(Integer)"), &Integer::operatorMul, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("%"), BEER_WIDEN("Integer::%(Integer)"), &Integer::operatorMod, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("/"), BEER_WIDEN("Integer::/(Integer)"), &Integer::operatorDiv, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("-"), BEER_WIDEN("Integer::-()"), &Integer::operatorMinus, 1, 0);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("=="), BEER_WIDEN("Integer::==(Integer)"), &Integer::operatorEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("!="), BEER_WIDEN("Integer::!=(Integer)"), &Integer::operatorNotEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("<"), BEER_WIDEN("Integer::<(Integer)"), &Integer::operatorSmaller, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("<="), BEER_WIDEN("Integer::<=(Integer)"), &Integer::operatorSmallerEqual, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN(">"), BEER_WIDEN("Integer::>(Integer)"), &Integer::operatorGreater, 1, 1);
	loader->addVirtualMethod(thread, klass, BEER_WIDEN(">="), BEER_WIDEN("Integer::>=(Integer)"), &Integer::operatorGreaterEqual, 1, 1);

	loader->addVirtualMethod(thread, klass, BEER_WIDEN("abs"), BEER_WIDEN("Integer::abs()"), &Integer::abs, 1, 0);

	loader->addOverrideMethod(thread, klass, BEER_WIDEN("String"), BEER_WIDEN("Object::String()"), &Integer::operatorString, 1, 0);
	loader->addOverrideMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Integer::createInstance, 1, 0);
}

