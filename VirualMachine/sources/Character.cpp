#include "stdafx.h"
#include "Character.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL Character::init(Thread* thread, StackFrame* frame, StackRef<Character> receiver, StackRef<Character> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Character::operatorString(Thread* thread, StackFrame* frame, StackRef<Character> receiver, StackRef<String> ret)
{
	StackRef<Integer> one(frame, frame->stackPush(Integer::makeInlineValue(1)));
	thread->createString(one, ret);

	Character::CharacterData c = receiver->getData();
	ret->copyData(&c, 1);
}

void BEER_CALL Character::operatorInteger(Thread* thread, StackFrame* frame, StackRef<Character> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData());
}

void BEER_CALL Character::operatorEqual(Thread* thread, StackFrame* frame, StackRef<Character> receiver, StackRef<Character> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Character::operatorNotEqual(Thread* thread, StackFrame* frame, StackRef<Character> receiver, StackRef<Character> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

Class* CharacterClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 5);
}

void CharacterClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->markAsValueType();
	klass->extends(0, vm->getObjectClass());
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Character")));
	method->setFunction(&Character::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::Character()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("String")));
	method->setFunction(&Character::operatorString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::String()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Integer")));
	method->setFunction(&Character::operatorInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::Integer()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("==")));
	method->setFunction(&Character::operatorEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::==(Character)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("!=")));
	method->setFunction(&Character::operatorNotEqual);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Character::!=(Character)")), method));
}
