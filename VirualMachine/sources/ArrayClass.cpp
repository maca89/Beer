#include "stdafx.h"
#include "ArrayClass.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "IntegerClass.h"
#include "Param.h"

using namespace Beer;


void BEER_CALL BeerArray_init(VirtualMachine* vm, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> size, StackRef<Array> ret)
{
	receiver->setSize(size->getData());
	ret = receiver;
}

void BEER_CALL BeerArray_getSize(VirtualMachine* vm, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> ret)
{
	ret = vm->createInteger(receiver->getSize());
}

void BEER_CALL BeerArray_get(VirtualMachine* vm, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> ret)
{
	BOUNDS_ASSERT(index->getData(), receiver->getSize());

	Integer* object = static_cast<Integer*>(receiver->getItem(index->getData()));

	if(object) ret = object;
	else
	{
		// just a temporary solution to missing value types
		ret = vm->createInteger(0);
	}
}

void BEER_CALL BeerArray_set(VirtualMachine* vm, StackFrame* frame, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Integer> object)
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


Object* ArrayClass::createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
{
	Integer* size = static_cast<Integer*>(frame->stackTop());
	Array* arr = gc->alloc<Array>(
		Object::OBJECT_CHILDREN_COUNT + static_cast<int32>(size->getData())
	);
	arr->setSize(size->getData());
	arr->setClass(this);
	arr->setItemClass(NULL); // TODO
	return arr;
}

/*Object* ArrayClass::cloneShallow(VirtualMachine* vm, Object* object, StackFrame* frame, GarbageCollector* gc)
{
	Array* arr = gc->alloc<Array>(static_cast<int32>(object->getInstance<Array>()->getSize()));
	arr->setClass(this);

	// didnt call ClassReflection:cloneShallow, co properties werent copied
	for(uint32 i = 0; i < getPropertiesCount(); i++)
	{
		arr->setChild(i, object->getChild(i)); // array elements are copied as well
	}
	return arr;
}*/

Class* ArrayClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<ArrayClass>(name, 1, 0, 4);
}

void ArrayClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	Class* objectClass = vm->getObjectClass();
	//ClassReflection* integerClass = vm->getIntegerClass();

	klass->extends(0, objectClass);
	
	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("Array")));
	//method->getReturn(0)->setType(klass);
	//method->getParam(0)->setType(integerClass);
	method->setFunction(&BeerArray_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::Array(Integer)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("getLength")));
	//method->getReturn(0)->setType(integerClass);
	method->setFunction(&BeerArray_getSize);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::getLength()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("get")));
	//method->getReturn(0)->setType(objectClass);
	//method->getParam(0)->setType(integerClass);
	method->setFunction(&BeerArray_get);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::get(Integer)")), method));

	method = loader->createMethod(0, 2);
	method->setName(vm->createString(BEER_WIDEN("set")));
	//method->getParam(0)->setType(integerClass);
	//method->getParam(1)->setType(objectClass);
	method->setFunction(&BeerArray_set);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Array::set(Integer,Integer)")), method));
}