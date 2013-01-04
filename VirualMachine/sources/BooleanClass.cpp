#include "stdafx.h"
#include "BooleanClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param)																	\
struct BeerBoolean_CompareOperator##Name																				\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Boolean> receiver, 																					\
		StackRef<Param> arg, 																							\
		StackRef<Boolean> ret)																							\
	{																													\
		ret = Boolean::makeInlineValue(receiver->getData() Operator arg->getData());									\
	}																													\
};																														\

#define BuildCompareOperator(Class, Name, Operator, Param, Do)															\
	BuildCompareOperatorFn(Name, Do, Param);																			\
	MethodReflection* Name##Method																						\
		= loader->createMethod<MethodReflection>(																		\
			BEER_WIDEN(#Operator), 																						\
			string(Class->getName()) + BEER_WIDEN("::") + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")"), 		\
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
	ret = Boolean::makeInlineValue(!receiver->getData());
}

ClassReflection* BooleanClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<BooleanClass>(name, 1, 0, 6);
}

void BooleanClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->markAsValueType();
	klass->extends(0, vm->getObjectClass());
	
	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Boolean"), string(klass->getName()) + BEER_WIDEN("::Boolean()"), 1, 0);
	method->setFunction(&BeerBoolean_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Boolean"), string(klass->getName()) + BEER_WIDEN("::!()"), 1, 0);
	method->setFunction(&BeerBoolean_Negation);
	klass->setMethod(methodi++, method);

	BuildCompareOperator(klass, Equal, ==, Boolean, ==);
	BuildCompareOperator(klass, NotEqual, !=, Boolean, !=);
	BuildCompareOperator(klass, Or, ||, Boolean, ||);
	BuildCompareOperator(klass, And, &&, Boolean, &&);
}
