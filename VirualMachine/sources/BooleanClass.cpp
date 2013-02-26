#include "stdafx.h"
#include "BooleanClass.h"
#include "Method.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param)																	\
struct BeerBoolean_CompareOperator##Name																				\
{																														\
	static void BEER_CALL fn(																							\
		Thread* thread,		 																							\
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
	method = loader->createMethod(																				\
			1, 																											\
			1																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));															\
	/*method->getReturn(0)->setType(integerClass);*/																	\
	/*method->getParam(0)->setType(integerClass);*/																		\
	method->setFunction(&(BeerBoolean_CompareOperator##Name::fn));														\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Boolean::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")")).c_str()), method));\



void BEER_CALL BeerBoolean_init(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerBoolean_Negation(Thread* thread, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(!receiver->getData());
}

Class* BooleanClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<BooleanClass>(name, 1, 0, 6);
}

void BooleanClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	klass->extends(0, vm->getObjectClass());
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Boolean")));
	method->setFunction(&BeerBoolean_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::Boolean()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("!")));
	method->setFunction(&BeerBoolean_Negation);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Boolean::!()")), method));

	BuildCompareOperator(klass, Equal, ==, Boolean, ==);
	BuildCompareOperator(klass, NotEqual, !=, Boolean, !=);
	BuildCompareOperator(klass, Or, ||, Boolean, ||);
	BuildCompareOperator(klass, And, &&, Boolean, &&);
}
