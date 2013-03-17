#include "stdafx.h"
#include "Pair.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Frame.h"
#include "Integer.h"
#include "Param.h"

using namespace Beer;

void BEER_CALL Pair::init(Thread* thread, StackRef<Pair> receiver, StackRef<Pair> ret)
{
	ret = receiver;
}

void BEER_CALL Pair::init_ObjectObject(Thread* thread, StackRef<Pair> receiver, StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret)
{
	Pair::setFirst(thread, receiver, first);
	Pair::setSecond(thread, receiver, second);
	ret = receiver;
}

void BEER_CALL Pair::getFirst(Thread* thread, StackRef<Pair> receiver, StackRef<Object> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_PAIR_FIRST);
}

void BEER_CALL Pair::setFirst(Thread* thread, StackRef<Pair> receiver, StackRef<Object> value)
{
	Object::setChild(thread, receiver, value, CHILD_ID_PAIR_FIRST);
}

void BEER_CALL Pair::getSecond(Thread* thread, StackRef<Pair> receiver, StackRef<Object> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_PAIR_SECOND);
}

void BEER_CALL Pair::setSecond(Thread* thread, StackRef<Pair> receiver, StackRef<Object> value)
{
	Object::setChild(thread, receiver, value, CHILD_ID_PAIR_SECOND);
}

void PairClassInitializer::createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret)
{
	return loader->createClass<Class>(thread, name, ret, 1, 2, 6 + Object::OBJECT_METHODS_COUNT);
}

void PairClassInitializer::initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	{
		StackRef<Class> objectClass(frame, frame->stackPush());
		thread->getObjectClass(objectClass);
		Class::addParent(thread, klass, objectClass);
		frame->stackMoveTop(-1); //  pop objectClass
	}

	loader->addMethod(thread, klass, BEER_WIDEN("Pair"), BEER_WIDEN("Pair::Pair()"), &Pair::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("Pair"), BEER_WIDEN("Pair::Pair(Object,Object)"), &Pair::init_ObjectObject, 1, 2);
	loader->addMethod(thread, klass, BEER_WIDEN("getFirst"), BEER_WIDEN("Pair::getFirst()"), &Pair::getFirst, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("setFirst"), BEER_WIDEN("Pair::setFirst(Object)"), &Pair::setFirst, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("getSecond"), BEER_WIDEN("Pair::getSecond()"), &Pair::getSecond, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("setSecond"), BEER_WIDEN("Pair::setSecond(Object)"), &Pair::setSecond, 0, 1);
}