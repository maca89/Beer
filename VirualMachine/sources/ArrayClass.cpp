#include "stdafx.h"
#include "ArrayClass.h"
#include "MethodReflection.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "IntegerClass.h"
#include "ParamReflection.h"

using namespace Beer;

#ifdef BEER_BOUNDS_ASSERT_ON
#	define BOUNDS_ASSERT(index, size) if(index < 0 || index >= size) throw RuntimeException(std::string("Array index out of bounds ") /*+ index + " / " + size*/);
#else
#	define BOUNDS_ASSERT(index, size)
#endif


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

	/*Array* arr = receiver.get();
	Integer::IntegerData i = index->getData();*/
	Integer* object = static_cast<Integer*>(receiver->getItem(index->getData()));
	
	/*if(object == reinterpret_cast<Object*>(0x0e))
	{
		Object* obj = receiver->getItem(i);
		std::cout << "!";
	}*/

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

	/*Array* arr = receiver.get();
	Integer::IntegerData i = index->getData();

	Object* obj = receiver->getItem(i);
	if(obj == reinterpret_cast<Object*>(0x0e))
	{
		std::cout << "!";
	}*/

	receiver->setItem(index->getData(), object.get());
}

void Array::toString(VirtualMachine* vm, std::string& out)
{
	out += "[";
	for(int32 i = 0; i < getSize(); i++)
	{
		Object* obj = getItem(i);
		if(obj)
		{
			std::stringstream n;
			vm->getClassTable()->translate(obj)->dump(obj, n);
			out += n.str();
		}
		else out += "0";

		if(i < getSize() - 1) out += ", ";
	}
	out += "]";
}


Object* ArrayClass::createInstance(StackFrame* frame, GarbageCollector* gc)
{
	Integer* size = static_cast<Integer*>(frame->stackTop());
	Array* arr = gc->alloc<Array>(static_cast<int32>(size->getData()));
	//arr->mSize
	arr->setClass(this);
	//arr->setItemClass(...);
	return arr;
}

Object* ArrayClass::cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
{
	Array* arr = gc->alloc<Array>(static_cast<int32>(object->getInstance<Array>()->getSize()));
	arr->setClass(this);

	// didnt call ClassReflection:cloneShallow, co properties werent copied
	for(uint32 i = 0; i < getPropertiesCount(); i++)
	{
		arr->setChild(i, object->getChild(i)); // array elements are copied as well
	}
	return arr;
}

ClassReflection* ArrayClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<ArrayClass>(name, 1, 0, 4);
}

void ArrayClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	ClassReflection* objectClass = vm->getObjectClass();
	ClassReflection* integerClass = vm->getIntegerClass();

	klass->extends(0, objectClass);
	
	MethodReflection* initMethod = loader->createMethod<MethodReflection>("Array", std::string(klass->getName()) + "::Array(Integer)", 1, 1);
	//initMethod->getReturn(0)->setType(klass);
	//initMethod->getParam(0)->setType(integerClass);
	initMethod->setFunction(&BeerArray_init);
	klass->setMethod(0, initMethod);

	MethodReflection* getSizeMethod = loader->createMethod<MethodReflection>("getLength", std::string(klass->getName()) + "::getLength()", 1, 0);
	//getSizeMethod->getReturn(0)->setType(integerClass);
	getSizeMethod->setFunction(&BeerArray_getSize);
	klass->setMethod(1, getSizeMethod);

	MethodReflection* getItemMethod = loader->createMethod<MethodReflection>("get", std::string(klass->getName()) + "::get(Integer)", 1, 1);
	//getItemMethod->getReturn(0)->setType(objectClass);
	//getItemMethod->getParam(0)->setType(integerClass);
	getItemMethod->setFunction(&BeerArray_get);
	klass->setMethod(2, getItemMethod);

	MethodReflection* setItemMethod = loader->createMethod<MethodReflection>("set", std::string(klass->getName()) + "::set(Integer,Integer)", 0, 2);
	//setItemMethod->getParam(0)->setType(integerClass);
	//setItemMethod->getParam(1)->setType(objectClass);
	setItemMethod->setFunction(&BeerArray_set);
	klass->setMethod(3, setItemMethod);
}