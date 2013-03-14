#include "stdafx.h"
#include "Array.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Frame.h"
#include "Integer.h"
#include "Param.h"

using namespace Beer;


void BEER_CALL Array::init(Thread* thread, StackRef<Array> receiver, StackRef<Integer> length, StackRef<Array> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

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
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	Integer::IntegerData itemIndex = index->getData();
	BOUNDS_ASSERT(itemIndex, receiver->getSize());

	Object::getChild(thread, receiver, ret, OBJECT_CHILDREN_COUNT + itemIndex);
}

void BEER_CALL Array::operatorSet(Thread* thread, StackRef<Array> receiver, StackRef<Integer> index, StackRef<Object> object)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	Integer::IntegerData itemIndex = index->getData();
	BOUNDS_ASSERT(itemIndex, receiver->getSize());

	Object::setChild(thread, receiver, object, OBJECT_CHILDREN_COUNT + itemIndex);
}

void BEER_CALL Array::operatorString(Thread* thread, StackRef<Array> receiver, StackRef<String> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Object> object(frame, frame->stackPush());
	StackRef<Integer> index(frame, frame->stackPush());
	StackRef<Integer> length(frame, frame->stackPush());

	Array::getLength(thread, receiver, length);

	string out = BEER_WIDEN("["); // TODO: use String*
	
	for(int32 i = 0; i < length->getData(); i++)
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

		if(i < length->getData() - 1) out += BEER_WIDEN(", ");
	}
	out += BEER_WIDEN("]");

	thread->createString(ret, out);

	frame->stackMoveTop(-3); // pop object, index, length
}

void BEER_CALL Array::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Array> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> length = StackRef<Integer>(frame, -2); // ctor parameter
	
	StackRef<Array> object(frame, frame->stackPush());
	thread->createArray(length, object);
	ret = object;

	frame->stackPop(); // pop object (is stored in ret)
}

void ArrayClassInitializer::createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret)
{
	loader->createClass<Class>(thread, name, ret, 1, 0, 6);
}

void ArrayClassInitializer::initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	{
		StackRef<Class> objectClass(frame, frame->stackPush());
		thread->getObjectClass(objectClass);
		Class::addParent(thread, klass, objectClass);
		frame->stackMoveTop(-1); //  pop objectClass
	}
	
	loader->addMethod(thread, klass, BEER_WIDEN("Array"), BEER_WIDEN("Array::Array(Integer)"), &Array::init, 1, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("getLength"), BEER_WIDEN("Array::getLength()"), &Array::getLength, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("get"), BEER_WIDEN("Array::get(Integer)"), &Array::operatorGet, 1, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("set"), BEER_WIDEN("Array::set(Integer,Integer)"), &Array::operatorSet, 0, 2);
	loader->addMethod(thread, klass, BEER_WIDEN("String"), BEER_WIDEN("Array::String()"), &Array::operatorString, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Array::createInstance, 1, 0);
}