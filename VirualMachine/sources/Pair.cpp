#include "stdafx.h"
#include "Pair.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Frame.h"
#include "Integer.h"
#include "Param.h"

using namespace Beer;


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
		frame->stackPop(); //  pop objectClass
	}

	loader->addMethod(thread, klass, BEER_WIDEN("Pair"), BEER_WIDEN("Pair::Pair()"), &Pair::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("Pair"), BEER_WIDEN("Pair::Pair(Object,Object)"), &Pair::init_ObjectObject, 1, 2);
	loader->addMethod(thread, klass, BEER_WIDEN("getFirst"), BEER_WIDEN("Pair::getFirst()"), &Pair::getFirst, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("setFirst"), BEER_WIDEN("Pair::setFirst(Object)"), &Pair::setFirst, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("getSecond"), BEER_WIDEN("Pair::getSecond()"), &Pair::getSecond, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("setSecond"), BEER_WIDEN("Pair::setSecond(Object)"), &Pair::setSecond, 0, 1);
}