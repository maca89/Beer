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
	uint16 methodi = 0;
	klass->extends(0, vm->getObjectClass());
	
	Method* method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Float")));
	method->setFunction(&Float::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::Float()")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Integer")));
	method->setFunction(&Float::operatorInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::Integer()")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&Float::operatorString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::String()")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&Float::operatorAdd);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::+(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("-")));
	method->setFunction(&Float::operatorSub);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::-(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("*")));
	method->setFunction(&Float::operatorMul);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::*(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("/")));
	method->setFunction(&Float::operatorDiv);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::/(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("==")));
	method->setFunction(&Float::operatorEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::==(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("!=")));
	method->setFunction(&Float::operatorNotEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::!=(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("<")));
	method->setFunction(&Float::operatorSmaller);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::<(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("<=")));
	method->setFunction(&Float::operatorSmallerEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::<=(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN(">")));
	method->setFunction(&Float::operatorGreater);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::>(Float)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN(">=")));
	method->setFunction(&Float::operatorGreaterEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::>=(Float)")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("-")));
	method->setFunction(&Float::operatorMinus);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Float::-()")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&Float::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
	
}