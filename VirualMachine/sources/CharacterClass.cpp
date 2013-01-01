#include "stdafx.h"
#include "CharacterClass.h"
#include "MethodReflection.h"
#include "StringClass.h"
#include "VirtualMachine.h"

using namespace Beer;


// compare operator

#define BuildCompareOperatorFn(Name, Operator, Param)																	\
struct BeerCharacter_CompareOperator##Name																				\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Character> receiver, 																					\
		StackRef<Param> arg, 																							\
		StackRef<Boolean> ret)																							\
	{																													\
		ret = Character::makeInlineValue(receiver->getData() Operator arg->getData());									\
	}																													\
};																														\

#define BuildCompareOperator(Class, Name, Operator, Param, Do)															\
	BuildCompareOperatorFn(Name, Do, Param);																			\
	MethodReflection* Name##Method																						\
		= loader->createMethod<MethodReflection>(																		\
			BEER_WIDEN(#Operator), 																						\
			string(Class->getName()) + BEER_WIDEN("::") + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")"), 	\
			1, 																											\
			1																											\
		);																												\
	/*Name##Method->getReturn(0)->setType(integerClass);*/																\
	/*Name##Method->getParam(0)->setType(integerClass);*/																\
	Name##Method->setFunction(&(BeerCharacter_CompareOperator##Name::fn));												\
	Class->setMethod(methodi++, Name##Method);																			\



void BEER_CALL BeerCharacter_init(VirtualMachine* vm, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Boolean> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerCharacter_toString(VirtualMachine* vm, StackFrame* frame, StackRef<Boolean> receiver, StackRef<String> ret)
{
	ret = vm->createString(receiver->getData());
}

void BEER_CALL BeerCharacter_toInteger(VirtualMachine* vm, StackFrame* frame, StackRef<Boolean> receiver, StackRef<Integer> ret)
{
	ret = vm->createInteger(receiver->getData());
}

ClassReflection* CharacterClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<CharacterClass>(name, 1, 0, 5);
}

void CharacterClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->markAsValueType();
	klass->extends(0, vm->getObjectClass());
	
	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Character"), string(klass->getName()) + BEER_WIDEN("::Character()"), 1, 0);
	method->setFunction(&BeerCharacter_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Character"), string(klass->getName()) + BEER_WIDEN("::String()"), 1, 0);
	method->setFunction(&BeerCharacter_toString);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("Character"), string(klass->getName()) + BEER_WIDEN("::Integer()"), 1, 0);
	method->setFunction(&BeerCharacter_toInteger);
	klass->setMethod(methodi++, method);

	BuildCompareOperator(klass, Equal, ==, Character, ==);
	BuildCompareOperator(klass, NotEqual, !=, Character, !=);
}
