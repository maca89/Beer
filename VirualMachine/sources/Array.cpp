#include "stdafx.h"
#include "Array.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "Integer.h"
#include "Param.h"

using namespace Beer;


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

void BEER_CALL Array::init(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> length, StackRef<Array> ret)
{
	receiver->setSize(length->getData());

#ifdef BEER_VALUE_TYPES_WORKAROUND
	// fix missing value types (limited to integers)
	Array::LengthData realLength = length->getData();
	Integer* zero = Integer::makeInlineValue(0);

	for(Array::LengthData i = 0; i < realLength; i++)
	{
		receiver->setItem(i, zero);
	}
#endif // BEER_VALUE_TYPES_WORKAROUND

	ret = receiver;
}

void BEER_CALL Array::getLength(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getSize());
}

void BEER_CALL Array::operatorGet(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> ret)
{
	BOUNDS_ASSERT(index->getData(), receiver->getSize());
	ret = static_cast<Integer*>(receiver->getItem(index->getData()));
}

void BEER_CALL Array::operatorSet(Thread* thread, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> object)
{
	BOUNDS_ASSERT(index->getData(), receiver->getSize());
	receiver->setItem(index->getData(), object.get());
}

void BEER_CALL Array::createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<Array> ret)
{
	StackRef<Integer> length = StackRef<Integer>(frame, -2); // ctor parameter
	
	StackRef<Array> object(frame, frame->stackPush());
	StackRef<Integer> copiedLength = length.push(frame);
	
	thread->createArray(copiedLength, object); // pops copied length
	ret = object;

	frame->stackPop(); // pop object (is stored in ret)
}

Class* ArrayClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 5);
}

void ArrayClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("Array")));
	method->setFunction(&Array::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::Array(Integer)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("getLength")));
	method->setFunction(&Array::getLength);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::getLength()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("get")));
	method->setFunction(&Array::operatorGet);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::get(Integer)")), method));

	method = loader->createMethod(0, 2);
	method->setName(vm->createString(BEER_WIDEN("set")));
	method->setFunction(&Array::operatorSet);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::set(Integer,Integer)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("createInstance")));
	method->setFunction(&Array::createInstance);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("$Class::createInstance()")), method));
}