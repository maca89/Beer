#include "stdafx.h"
#include "Array.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "Integer.h"
#include "Param.h"

using namespace Beer;


void Array::toString(Thread* thread, string& out)
{
	StackFrame* frame = thread->getStackFrame();

	StackRef<Object> object(frame, frame->stackPush());
	StackRef<Integer> index(frame, frame->stackPush());
	StackRef<Array> receiver(frame, frame->stackPush(this));

	out += BEER_WIDEN("[");
	for(int32 i = 0; i < getSize(); i++)
	{
		thread->createInteger(index, i);
		Array::operatorGet(thread, receiver, index, object);
		if(!object.isNull())
		{
			stringstream n;
			n << object.staticCast<Integer>()->getData(); // TODO: call Object::String()
			out += n.str();
		}
		else out += BEER_WIDEN("0");

		if(i < getSize() - 1) out += BEER_WIDEN(", ");
	}
	out += BEER_WIDEN("]");

	frame->stackMoveTop(-3); // pop object & index & receiver
}

void BEER_CALL Array::init(Thread* thread, StackRef<Array> receiver, StackRef<Integer> length, StackRef<Array> ret)
{
	StackFrame* frame = thread->getStackFrame();
	receiver->setSize(length->getData());

	/////////////////////////////////////////////////////////
	// fix missing value types (limited to integers)
	Array::LengthData realLength = length->getData();
	StackRef<Integer> zero(frame, frame->stackPush(Integer::makeInlineValue(0)));

	for(Array::LengthData i = 0; i < realLength; i++)
	{
		Object::setChild(thread, receiver, zero, OBJECT_CHILDREN_COUNT + i);
	}
	frame->stackMoveTop(-1); // pop zero & index
	/////////////////////////////////////////////////////////

	ret = receiver;
}

void BEER_CALL Array::getLength(Thread* thread, StackRef<Array> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->getSize());
}

void BEER_CALL Array::operatorGet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> ret)
{
	StackFrame* frame = thread->getStackFrame();

	Integer::IntegerData itemIndex = index->getData();
	BOUNDS_ASSERT(itemIndex, receiver->getSize());

	Object::getChild(thread, receiver, ret, OBJECT_CHILDREN_COUNT + itemIndex);
}

void BEER_CALL Array::operatorSet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> object)
{
	StackFrame* frame = thread->getStackFrame();

	Integer::IntegerData itemIndex = index->getData();
	BOUNDS_ASSERT(itemIndex, receiver->getSize());

	Object::setChild(thread, receiver, object, OBJECT_CHILDREN_COUNT + itemIndex);
}

void BEER_CALL Array::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Array> ret)
{
	StackFrame* frame = thread->getStackFrame();
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
	loader->extendClass(klass, vm->getObjectClass());
	
	loader->addMethod(klass, BEER_WIDEN("Array"), BEER_WIDEN("Array::Array(Integer)"), &Array::init, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("getLength"), BEER_WIDEN("Array::getLength()"), &Array::getLength, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("get"), BEER_WIDEN("Array::get(Integer)"), &Array::operatorGet, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("set"), BEER_WIDEN("Array::set(Integer,Integer)"), &Array::operatorSet, 0, 2);
	loader->addMethod(klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Array::createInstance, 1, 0);
}