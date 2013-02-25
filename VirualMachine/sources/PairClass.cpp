#include "stdafx.h"
#include "PairClass.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "IntegerClass.h"
#include "Param.h"

using namespace Beer;


void BEER_CALL BeerPair_init(VirtualMachine* vm, StackFrame* frame, StackRef<Pair> receiver, StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret)
{
	receiver->setFirst(*first);
	receiver->setSecond(*second);
	ret = receiver;
}

Object* PairClass::createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
{
	Pair* pair = gc->alloc<Pair>(
		Object::OBJECT_CHILDREN_COUNT + 2
	);
	pair->setClass(this);
	return pair;
}

Class* PairClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<PairClass>(name, 1, 2, 5); // TODO: at least methods: Pair(Object,Object) getFirst, setFirst, getSecond, setSecond
}

void PairClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("Pair")));
	//method->getReturn(0)->setType(klass);
	//method->getParam(0)->setType(integerClass);
	method->setFunction(&BeerPair_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Pair::Pair(Object,Object)")), method));
}