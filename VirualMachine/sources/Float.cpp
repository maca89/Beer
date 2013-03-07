#include "stdafx.h"
#include "Float.h"
#include "Integer.h"
#include "Boolean.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL Float::init(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> ret)
{
	ret = receiver;
}

void BEER_CALL Float::operatorInteger(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, static_cast<Integer::IntegerData>(receiver->getData()));
}

void BEER_CALL Float::operatorString(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<String> ret)
{
	stringstream ss;
	ss << std::setprecision(8) << std::fixed << receiver->getData();
	// TODO: check and throw Exception
	thread->createString(ret, ss.str());
}

void BEER_CALL Float::operatorAdd(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret)
{
	thread->createFloat(ret, arg->getData() + receiver->getData());
}

void BEER_CALL Float::operatorSub(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret)
{
	thread->createFloat(ret, arg->getData() - receiver->getData());
}

void BEER_CALL Float::operatorMul(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret)
{
	thread->createFloat(ret, arg->getData() * receiver->getData());
}

void BEER_CALL Float::operatorDiv(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Float> ret)
{
	thread->createFloat(ret, arg->getData() / receiver->getData());
}

void BEER_CALL Float::operatorMinus(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> ret)
{
	thread->createFloat(ret, -receiver->getData());
}

void BEER_CALL Float::operatorEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Float::operatorNotEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
}

void BEER_CALL Float::operatorSmaller(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() < arg->getData());
}

void BEER_CALL Float::operatorSmallerEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() <= arg->getData());
}

void BEER_CALL Float::operatorGreater(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() > arg->getData());
}

void BEER_CALL Float::operatorGreaterEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Float> receiver, StackRef<Float> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() >= arg->getData());
}

void BEER_CALL Float::createInstance(Thread* thread/*, StackFrame* frame*/, StackRef<Class> receiver, StackRef<Float> ret)
{
	thread->createFloat(ret, 0);
}

Class* FloatClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 15);
}

void FloatClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	loader->extendClass(klass, vm->getObjectClass());
	
	loader->addMethod(klass, BEER_WIDEN("Float"), BEER_WIDEN("Float::Float()"), &Float::init, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("Integer"), BEER_WIDEN("Float::Integer()"), &Float::operatorInteger, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("String"), BEER_WIDEN("Float::String()"), &Float::operatorString, 1, 0);

	loader->addMethod(klass, BEER_WIDEN("+"), BEER_WIDEN("Float::+(Float)"), &Float::operatorAdd, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("-"), BEER_WIDEN("Float::-(Float)"), &Float::operatorSub, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("*"), BEER_WIDEN("Float::*(Float)"), &Float::operatorMul, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("/"), BEER_WIDEN("Float::/(Float)"), &Float::operatorDiv, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("-"), BEER_WIDEN("Float::-()"), &Float::operatorMinus, 1, 0);

	loader->addMethod(klass, BEER_WIDEN("=="), BEER_WIDEN("Float::==(Float)"), &Float::operatorEqual, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("!="), BEER_WIDEN("Float::!=(Float)"), &Float::operatorNotEqual, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("<"), BEER_WIDEN("Float::<(Float)"), &Float::operatorSmaller, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("<="), BEER_WIDEN("Float::<=(Float)"), &Float::operatorSmallerEqual, 1, 1);
	loader->addMethod(klass, BEER_WIDEN(">"), BEER_WIDEN("Float::>(Float)"), &Float::operatorGreater, 1, 1);
	loader->addMethod(klass, BEER_WIDEN(">="), BEER_WIDEN("Float::>=(Float)"), &Float::operatorGreaterEqual, 1, 1);

	loader->addMethod(klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Float::createInstance, 1, 0);
}