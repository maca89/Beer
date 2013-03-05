#include "stdafx.h"
#include "Integer.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Param.h"
#include "Float.h"
#include "Boolean.h"

using namespace Beer;


void BEER_CALL Integer::init(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Integer::operatorFloat(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Float> ret)
{
	thread->createFloat(ret, static_cast<Float::FloatData>(receiver->getData()));
}

void BEER_CALL Integer::operatorString(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<String> ret)
{
	stringstream ss;
	ss << receiver->getData();
	// TODO: check and throw Exception
	ret = thread->getVM()->createString(ss.str());
}

void BEER_CALL Integer::operatorAdd(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() + arg->getData());
}

void BEER_CALL Integer::operatorSub(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() - arg->getData());
}

void BEER_CALL Integer::operatorMul(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() * arg->getData());
}

void BEER_CALL Integer::operatorMod(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData() % arg->getData());
}

void BEER_CALL Integer::operatorDiv(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Float> ret)
{
	thread->createFloat(ret, ((Float::FloatData)receiver->getData()) / arg->getData());
}

void BEER_CALL Integer::operatorMinus(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, -receiver->getData());
}

void BEER_CALL Integer::operatorEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Integer::operatorNotEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
}

void BEER_CALL Integer::operatorSmaller(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() < arg->getData());
}

void BEER_CALL Integer::operatorSmallerEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() <= arg->getData());
}

void BEER_CALL Integer::operatorGreater(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() > arg->getData());
}

void BEER_CALL Integer::operatorGreaterEqual(Thread* thread/*, StackFrame* frame*/, StackRef<Integer> receiver, StackRef<Integer> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() >= arg->getData());
}

void BEER_CALL Integer::createInstance(Thread* thread/*, StackFrame* frame*/, StackRef<Class> receiver, StackRef<Integer> ret)
{
	ret = Integer::makeInlineValue(0);
}

Class* IntegerClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 17);
}

void IntegerClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	uint16 methodi = 0;
	Method* method = NULL;
	Class* objectClass = vm->getObjectClass();

	klass->extends(methodi++, objectClass);
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Integer")));
	method->setFunction(&Integer::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::Integer()")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Float")));
	method->setFunction(&Integer::operatorFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::Float()")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&Integer::operatorString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::String()")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("+")));
	method->setFunction(&Integer::operatorAdd);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::+(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("-")));
	method->setFunction(&Integer::operatorSub);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::-(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("*")));
	method->setFunction(&Integer::operatorMul);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::*(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("%")));
	method->setFunction(&Integer::operatorMod);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::%(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("/")));
	method->setFunction(&Integer::operatorDiv);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::/(Integer)")), method));
	
	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("-")));
	method->setFunction(&Integer::operatorMinus);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::-()")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("==")));
	method->setFunction(&Integer::operatorEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::==(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("!=")));
	method->setFunction(&Integer::operatorNotEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::!=(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("<")));
	method->setFunction(&Integer::operatorSmaller);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::<(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("<=")));
	method->setFunction(&Integer::operatorSmallerEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::<=(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN(">")));
	method->setFunction(&Integer::operatorGreater);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::>(Integer)")), method));
	
	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN(">=")));
	method->setFunction(&Integer::operatorGreaterEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Integer::>=(Integer)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&Integer::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
}

