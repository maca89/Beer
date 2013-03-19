#include "stdafx.h"
#include "Class.h"
#include "Object.h"
#include "Method.h"
#include "Property.h"
#include "String.h"
#include "VirtualMachine.h"
#include "Thread.h"
#include "Heap.h"

using namespace Beer;


void BEER_CALL Class::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(thread, receiver, propertiesCount);

	ret = thread->getHeap()->alloc<Object>(
		static_cast<uint32>(Object::OBJECT_CHILDREN_COUNT + propertiesCount->getData())
	);

	if(ret.isNull())
	{
		throw NotEnoughMemoryException(BEER_WIDEN("Unable to create new instance - Not enough memory"));
	}
	
	Object::setType(thread, ret, receiver);

	frame->stackMoveTop(-1); // pop propertiesCount
}

void BEER_CALL Class::findMethodIndex(Thread* thread, StackRef<Class> receiver, StackRef<String> selector, StackRef<Method> ret1, StackRef<Integer> ret2)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// try myself
	{
		StackRef<String> otherSelector(frame, frame->stackPush());
		StackRef<Pair> pair(frame, frame->stackPush());
		StackRef<Integer> index(frame, frame->stackPush());

		StackRef<Integer> methodsCount(frame, frame->stackPush());
		Class::getMethodsCount(thread, receiver, methodsCount);

		for(Integer::IntegerData i = methodsCount->getData() - 1; i >= 0; i--)
		{
			thread->createInteger(index, i);
			Class::getMethod(thread, receiver, index, pair);

			if(!pair.isNull())
			{
				Pair::getFirst(thread, pair, otherSelector);
			
				if(selector->compare(*otherSelector) == 0)
				{
					Pair::getSecond(thread, pair, ret1);
					ret2 = index;
					break; // found!
				}
			}
		}

		frame->stackMoveTop(-4); // pop otherSelector, pair, index, methodsCount
	}
}

void BEER_CALL Class::findMethod(Thread* thread, StackRef<Class> receiver, StackRef<String> selector, StackRef<Method> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> index(frame, frame->stackPush());
	Class::findMethodIndex(thread, receiver, selector, ret, index);

	frame->stackMoveTop(-1); // pop index
}

void BEER_CALL Class::substituable(Thread* thread, StackRef<Class> receiver, StackRef<Class> otherClass, StackRef<Boolean> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// is otherClass the same class?
	if(*receiver == *otherClass)
	{
		ret = Boolean::makeInlineValue(true);
		return;
	}

	// check parents
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Class> parent(frame, frame->stackPush());
		StackRef<Boolean> substituable(frame, frame->stackPush());

		StackRef<Integer> parentsCount(frame, frame->stackPush());
		Class::getParentsCount(thread, receiver, parentsCount);

		for(uint16 i = 0; i < parentsCount->getData(); i++)
		{
			thread->createInteger(index, i);
			Class::getParent(thread, receiver, index, parent);

			// parent of myself
			if(*parent == *receiver)
			{
				continue;
			}

			Class::substituable(thread, parent, otherClass, substituable);

			// is parent substituable?
			if(substituable->getData())
			{
				ret = Boolean::makeInlineValue(true);
				frame->stackMoveTop(-4); // pop index, parent, substituable, parentsCount
				return;
			}
		}

		frame->stackMoveTop(-4); // pop index, parent, substituable, parentsCount
	}

	// is not substituable
	ret = Boolean::makeInlineValue(false);
}

void BEER_CALL Class::getName(Thread* thread, StackRef<Class> receiver, StackRef<String> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME);
}

void BEER_CALL Class::setName(Thread* thread, StackRef<Class> receiver, StackRef<String> value)
{
	Object::setChild(thread, receiver, value, CHILD_ID_CLASS_NAME);
}

void BEER_CALL Class::getParent(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Class> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> parentsCount(frame, frame->stackPush());
	Class::getParentsCount(thread, receiver, parentsCount);

	BOUNDS_ASSERT(index->getData(), parentsCount->getData());

	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME + 1 + index->getData()); // 1 for name 

	frame->stackMoveTop(-1); // pop parentsCount
}

void BEER_CALL Class::getParentsCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mParentsCount); // TODO
}

void BEER_CALL Class::addParent(Thread* thread, StackRef<Class> receiver, StackRef<Class> value)
{
	DBG_ASSERT(hasParentFreeSlot(thread, receiver), BEER_WIDEN("Unable to add more parents"));

	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> parentNext(frame, frame->stackPush());
	Class::incrParentNext(thread, receiver, parentNext);

	// set child
	Object::setChild(thread, receiver, value, CHILD_ID_CLASS_NAME + 1 + parentNext->getData()); // 1 for name
	frame->stackMoveTop(-1); // pop parentNext

	// copy methods
	if(*receiver != *value){
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Pair> method(frame, frame->stackPush());

		StackRef<Integer> methodsCount(frame, frame->stackPush());
		Class::getMethodsCount(thread, value, methodsCount);

		for(uint16 i = 0; i < methodsCount->getData(); i++)
		{
			thread->createInteger(index, i);
			Class::getMethod(thread, value, index, method);
			Class::addMethod(thread, receiver, method);
		}

		frame->stackMoveTop(-3); // pop index, method, methodsCount
	}

	// copy properties
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Property> prop(frame, frame->stackPush());

		StackRef<Integer> propertiesCount(frame, frame->stackPush());
		Class::getPropertiesCount(thread, value, propertiesCount);

		for(uint16 i = 0; i < propertiesCount->getData(); i++)
		{
			thread->createInteger(index, i);
			Class::getProperty(thread, value, index, prop);
			Class::addProperty(thread, receiver, prop);
		}

		frame->stackMoveTop(-3); // pop prop, index, propertiesCount
	}
}

void BEER_CALL Class::getMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Pair> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> methodsCount(frame, frame->stackPush());
	Class::getMethodsCount(thread, receiver, methodsCount);

	StackRef<Integer> parentsCount(frame, frame->stackPush());
	Class::getParentsCount(thread, receiver, parentsCount);

	BOUNDS_ASSERT(index->getData(), methodsCount->getData());

	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME + 1 + parentsCount->getData() + index->getData()); // 1 for name

	frame->stackMoveTop(-2); // pop parentsCount, methodsCount
}

/*void Class::getOnlyMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Method> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Pair> pair(frame, frame->stackPush());
	Class::getMethod(thread, receiver, index, pair);

	Pair::getSecond(thread, pair, ret);

	frame->stackMoveTop(-1); // pop pair
}*/

void BEER_CALL Class::getMethodsCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mMethodsCount); // TODO
}

void BEER_CALL Class::addMethod(Thread* thread, StackRef<Class> receiver, StackRef<Pair> value)
{
	DBG_ASSERT(hasMethodFreeSlot(thread, receiver), BEER_WIDEN("Unable to add more methods"));

	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> parentsCount(frame, frame->stackPush());
	Class::getParentsCount(thread, receiver, parentsCount);

	StackRef<Integer> methodNext(frame, frame->stackPush());
	Class::incrMethodNext(thread, receiver, methodNext);

	Object::setChild(thread, receiver, value, CHILD_ID_CLASS_NAME + 1 + parentsCount->getData() + methodNext->getData()); // 1 for name

	frame->stackMoveTop(-2); // pop parentsCount, methodNext
}

void BEER_CALL Class::getProperty(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Property> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> methodsCount(frame, frame->stackPush());
	Class::getMethodsCount(thread, receiver, methodsCount);

	StackRef<Integer> parentsCount(frame, frame->stackPush());
	Class::getParentsCount(thread, receiver, parentsCount);

	StackRef<Integer> propertiesCount(frame, frame->stackPush());
	Class::getPropertiesCount(thread, receiver, propertiesCount);

	BOUNDS_ASSERT(index->getData(), propertiesCount->getData());

	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME + 1 + parentsCount->getData() + methodsCount->getData() + index->getData()); // 1 for name 

	frame->stackMoveTop(-3); // pop methodsCount, parentsCount, propertiesCount
}

void BEER_CALL Class::getPropertiesCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mPropertiesCount); // TODO
}

void BEER_CALL Class::addProperty(Thread* thread, StackRef<Class> receiver, StackRef<Property> value)
{
	DBG_ASSERT(hasPropertyFreeSlot(thread, receiver), BEER_WIDEN("Unable to add more properties"));

	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> methodsCount(frame, frame->stackPush());
	Class::getMethodsCount(thread, receiver, methodsCount);

	StackRef<Integer> parentsCount(frame, frame->stackPush());
	Class::getParentsCount(thread, receiver, parentsCount);

	StackRef<Integer> propertyNext(frame, frame->stackPush());
	Class::incrPropertyNext(thread, receiver, propertyNext);

	Object::setChild(thread, receiver, value, 
		CHILD_ID_CLASS_NAME + 1 + parentsCount->getData() + methodsCount->getData() + propertyNext->getData()
	); // 1 for name

	frame->stackMoveTop(-3); // pop methodsCount, parentsCount, propertyNext
}

void BEER_CALL Class::getPropertyNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mPropertyNext); // TODO
}

void BEER_CALL Class::getMethodNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mMethodNext); // TODO
}

void BEER_CALL Class::getParentNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mParentNext); // TODO
}

void BEER_CALL Class::incrPropertyNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mPropertyNext++); // TODO
}

void BEER_CALL Class::incrMethodNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mMethodNext++); // TODO
}

void BEER_CALL Class::incrParentNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mParentNext++); // TODO
}

bool Class::hasPropertyFreeSlot(Thread* thread, StackRef<Class> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> next(frame, frame->stackPush());
	StackRef<Integer> count(frame, frame->stackPush());

	Class::getPropertiesCount(thread, receiver, count);
	Class::getPropertyNext(thread, receiver, next);

	bool result = next->getData() < count->getData();
	frame->stackMoveTop(-2); // pop next, count
	return result;
}

bool Class::hasMethodFreeSlot(Thread* thread, StackRef<Class> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> next(frame, frame->stackPush());
	StackRef<Integer> count(frame, frame->stackPush());

	Class::getMethodsCount(thread, receiver, count);
	Class::getMethodNext(thread, receiver, next);

	bool result = next->getData() < count->getData();
	frame->stackMoveTop(-2); // pop next, count
	return result;
}

bool Class::hasParentFreeSlot(Thread* thread, StackRef<Class> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> next(frame, frame->stackPush());
	StackRef<Integer> count(frame, frame->stackPush());

	Class::getParentsCount(thread, receiver, count);
	Class::getParentNext(thread, receiver, next);

	bool result = next->getData() < count->getData();
	frame->stackMoveTop(-2); // pop next, count
	return result;
}