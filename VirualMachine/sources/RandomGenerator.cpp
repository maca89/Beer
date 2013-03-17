#include "stdafx.h"
#include "RandomGenerator.h"
#include "Heap.h"
#include "Float.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL RandomGenerator::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<RandomGenerator> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(thread, receiver, propertiesCount);

	ret = thread->getHeap()->alloc<RandomGenerator>(
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + propertiesCount->getData())
	);

	Class::setClass(thread, ret, receiver);

	frame->stackMoveTop(-1); // pop propertiesCount
}

void BEER_CALL RandomGenerator::init(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<RandomGenerator> ret)
{
	receiver->mGenerator = MerseneTwisterRandom();
	ret = receiver;
}

void BEER_CALL RandomGenerator::initInteger(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Integer> seed, StackRef<RandomGenerator> ret)
{
	Integer::IntegerData s = seed->getData();
	receiver->mGenerator = MerseneTwisterRandom(static_cast<int32>(s)); // TODO: 64bit seed
	ret = receiver;
}

void BEER_CALL RandomGenerator::operatorInteger(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Integer> ret)	
{
	int64 random = receiver->mGenerator.generate();
	thread->createInteger(ret, static_cast<Integer::IntegerData>(random));
}

void BEER_CALL RandomGenerator::operatorBoolean(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Boolean> ret)
{
	int64 random = (receiver->mGenerator.generate() % 2);
	ret = Boolean::makeInlineValue(random == 1 ? true : false);
}

void BEER_CALL RandomGenerator::operatorFloat(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Float> ret)
{
	int64 random = receiver->mGenerator.generate();
	thread->createFloat(ret, static_cast<Float::FloatData>(random));
}

void BEER_CALL RandomGenerator::operatorCharacter(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Character> ret)
{
	int64 random = receiver->mGenerator.generate();
	ret = Character::makeInlineValue(static_cast<Character::CharacterData>(random));
}

void BEER_CALL RandomGenerator::toss(Thread* thread, StackRef<RandomGenerator> receiver, StackRef<Float> probability, StackRef<Boolean> ret)
{
	Float::FloatData p = probability->getData();

	int64 random = receiver->mGenerator.generate() % Integer::MaxValue;
	int64 max = (int64)(p * Integer::MaxValue);

	ret = Boolean::makeInlineValue(random < max);
}

void RandomGeneratorClassInitializer::createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret)
{
	return loader->createClass<Class>(thread, name, ret, 1, 0, 8 + Object::OBJECT_METHODS_COUNT);
}

void RandomGeneratorClassInitializer::initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	{
		StackRef<Class> objectClass(frame, frame->stackPush());
		thread->getObjectClass(objectClass);
		Class::addParent(thread, klass, objectClass);
		frame->stackMoveTop(-1); //  pop objectClass
	}
	
	loader->addMethod(thread, klass, BEER_WIDEN("RandomGenerator"), BEER_WIDEN("RandomGenerator::RandomGenerator()"), &RandomGenerator::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("RandomGenerator"), BEER_WIDEN("RandomGenerator::RandomGenerator(Integer)"), &RandomGenerator::initInteger, 1, 1);

	loader->addMethod(thread, klass, BEER_WIDEN("Integer"), BEER_WIDEN("RandomGenerator::Integer()"), &RandomGenerator::operatorInteger, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("Boolean"), BEER_WIDEN("RandomGenerator::Boolean()"), &RandomGenerator::operatorBoolean, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("Float"), BEER_WIDEN("RandomGenerator::Float()"), &RandomGenerator::operatorFloat, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("Character"), BEER_WIDEN("RandomGenerator::Character()"), &RandomGenerator::operatorCharacter, 1, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("toss"), BEER_WIDEN("RandomGenerator::toss(Float)"), &RandomGenerator::toss, 1, 1);

	loader->addMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &RandomGenerator::createInstance, 1, 0);
}