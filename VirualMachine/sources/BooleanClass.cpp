#include "stdafx.h"
#include "BooleanClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param, Return)															\
struct BeerBoolean_CompareOperator##Name																				\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Boolean> receiver, 																					\
		StackRef<Param> arg, 																							\
		StackRef<Return> ret)																							\
	{																													\
		ret = vm->getClass(#Return)->createInstance<Return>(frame, vm->getHeap());										\
		ret->setData(static_cast<Return::Return##Data>(receiver->getData() Operator arg->getData()));					\
	}																													\
};																														\

#define BuildCompareOperator(Class, Name, Operator, Param, Return, Do)													\
	BuildCompareOperatorFn(Name, Do, Param, Return);																	\
	MethodReflection* Name##Method																						\
		= loader->createMethod<MethodReflection>(																		\
			#Operator, 																									\
			std::string(Class->getName()) + "::" + #Operator + "(" + #Param + ")", 										\
			1, 																											\
			1																											\
		);																												\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	Name##Method->setFunction(&(BeerBoolean_CompareOperator##Name::fn));												\
	Class->setMethod(methodi++, Name##Method);																			\



void BEER_CALL BeerBoolean_init(VirtualMachine* vm, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerBoolean_Negation(VirtualMachine* vm, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret)
{
	ret = vm->createBoolean(!receiver->getData());
}

ClassReflection* BooleanClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<BooleanClass>(name, 1, 0, 6);
}

void BooleanClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->markAsValueType();
	klass->extends(0, vm->getClass("Object"));
	
	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>("Boolean", std::string(klass->getName()) + "::Boolean()", 1, 0);
	method->setFunction(&BeerBoolean_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("Boolean", std::string(klass->getName()) + "::!()", 1, 0);
	method->setFunction(&BeerBoolean_Negation);
	klass->setMethod(methodi++, method);

	BuildCompareOperator(klass, Equal, ==, Boolean, Boolean, ==);
	BuildCompareOperator(klass, NotEqual, !=, Boolean, Boolean, !=);
	BuildCompareOperator(klass, Or, ||, Boolean, Boolean, ||);
	BuildCompareOperator(klass, And, &&, Boolean, Boolean, &&);
}
