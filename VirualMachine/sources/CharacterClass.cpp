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
	method = loader->createMethod<MethodReflection>(																	\
			1, 																											\
			1																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN(#Operator)));															\
	/*method->getReturn(0)->setType(integerClass);*/																	\
	/*method->getParam(0)->setType(integerClass);*/																		\
	method->setFunction(&(BeerCharacter_CompareOperator##Name::fn));													\
	Class->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Character::")) + BEER_WIDEN(#Operator) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")")).c_str()), method));\



void BEER_CALL BeerCharacter_init(VirtualMachine* vm, StackFrame* frame, StackRef<Character> receiver, StackRef<Character> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerCharacter_toString(VirtualMachine* vm, StackFrame* frame, StackRef<Character> receiver, StackRef<String> ret)
{
	ret = vm->createString(receiver->getData());
}

void BEER_CALL BeerCharacter_toInteger(VirtualMachine* vm, StackFrame* frame, StackRef<Character> receiver, StackRef<Integer> ret)
{
	ret = vm->createInteger(receiver->getData());
}

Class* CharacterClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<CharacterClass>(name, 1, 0, 5);
}

void CharacterClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	klass->extends(0, vm->getObjectClass());
	
	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Character")));
	method->setFunction(&BeerCharacter_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::Character()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&BeerCharacter_toString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::String()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Integer")));
	method->setFunction(&BeerCharacter_toInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::Integer()")), method));

	BuildCompareOperator(klass, Equal, ==, Character, ==);
	BuildCompareOperator(klass, NotEqual, !=, Character, !=);
}
