#include "stdafx.h"
#include "ArrayClass.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "IntegerClass.h"
#include "Param.h"

using namespace Beer;


void BEER_CALL BeerArray_init(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> size, StackRef<Array> ret)
{
	receiver->setSize(size->getData());
	ret = receiver;
}

void BEER_CALL BeerArray_getSize(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getSize());
}

void BEER_CALL BeerArray_get(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> ret)
{
	BOUNDS_ASSERT(index->getData(), receiver->getSize());

	Integer* object = static_cast<Integer*>(receiver->getItem(index->getData()));

	if(object) ret = object;
	else
	{
		// just a temporary solution to missing value types
		thread->createInteger(ret, 0);
	}
}

void BEER_CALL BeerArray_set(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> object)
{
	BOUNDS_ASSERT(index->getData(), receiver->getSize());

	receiver->setItem(index->getData(), object.get());
}

void Array::toString(VirtualMachine* vm, string& out)
{
	out += BEER_WIDEN("[");
	for(int32 i = 0; i < getSize(); i++)
	{
		Object* obj = getItem(i);
		if(obj)
		{
			stringstream n;
			n << ((Integer*)obj)->getData(); //vm->getClass(obj)->dump(obj, n); // TODO: call String()
			out += n.str();
		}
		else out += BEER_WIDEN("0");

		if(i < getSize() - 1) out += BEER_WIDEN(", ");
	}
	out += BEER_WIDEN("]");
}

void BEER_CALL ArrayClass::createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<Array> ret)
{
	// TODO: probably not working
	StackRef<Integer> length = StackRef<Integer>(frame, -2);
	thread->createArray(length, ret);
}

Class* ArrayClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<ArrayClass>(name, 1, 0, 5);
}

void ArrayClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("Array")));
	method->setFunction(&BeerArray_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::Array(Integer)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("getLength")));
	method->setFunction(&BeerArray_getSize);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::getLength()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("get")));
	method->setFunction(&BeerArray_get);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::get(Integer)")), method));

	method = loader->createMethod(0, 2);
	method->setName(vm->createString(BEER_WIDEN("set")));
	method->setFunction(&BeerArray_set);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::set(Integer,Integer)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&ArrayClass::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
}