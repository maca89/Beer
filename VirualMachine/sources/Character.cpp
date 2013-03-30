#include "stdafx.h"
#include "Character.h"
#include "Method.h"
#include "String.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL Character::init(Thread* thread, StackRef<Character> receiver, StackRef<Character> ret1)
{
	ret1 = receiver;
}

void BEER_CALL Character::operatorString(Thread* thread, StackRef<Character> receiver, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> one(frame, frame->stackPush(Integer::makeInlineValue(1)));
	thread->createString(one, ret);

	Character::CharacterData c = receiver->getData();
	ret->copyData(&c, 1);
}

void BEER_CALL Character::operatorInteger(Thread* thread, StackRef<Character> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getData());
}

void BEER_CALL Character::operatorEqual(Thread* thread, StackRef<Character> receiver, StackRef<Character> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
}

void BEER_CALL Character::operatorNotEqual(Thread* thread, StackRef<Character> receiver, StackRef<Character> arg, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
}

void BEER_CALL Character::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Character> ret)
{
	ret = Character::makeInlineValue(' ');
}

Class* CharacterClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, 0, 6 + Object::OBJECT_METHODS_COUNT);
}

void CharacterClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->addParent(thread->getVM()->getObjectClass());

	klass->markAsValueType();
	
	loader->addMethod(thread, klass, BEER_WIDEN("Character"), BEER_WIDEN("Character::Character()"), &Character::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("String"), BEER_WIDEN("Character::String()"), &Character::operatorString, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("Integer"), BEER_WIDEN("Character::Integer()"), &Character::operatorInteger, 1, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("=="), BEER_WIDEN("Character::==(Character)"), &Character::operatorEqual, 1, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("!="), BEER_WIDEN("Character::!=(Character)"), &Character::operatorNotEqual, 1, 1);

	loader->addMethod(thread, klass, BEER_WIDEN("Character"), BEER_WIDEN("$Class::createInstance()"), &Character::createInstance, 1, 0);
}
