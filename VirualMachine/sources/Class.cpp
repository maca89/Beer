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
	ret = thread->getHeap()->alloc<Object>(
		Object::OBJECT_CHILDREN_COUNT + receiver->getPropertiesCount()
	);

	if(ret.isNull())
	{
		throw NotEnoughMemoryException(BEER_WIDEN("Unable to create new instance - Not enough memory"));
	}
	
	Object::setClass(thread, ret, receiver);
}

void BEER_CALL Class::findMethod(Thread* thread, StackRef<Class> receiver, StackRef<String> selector, StackRef<Method> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// try myself
	{
		StackRef<String> otherSelector(frame, frame->stackPush());
		StackRef<Pair> pair(frame, frame->stackPush());
		StackRef<Integer> index(frame, frame->stackPush());

		for(uint16 i = 0; i < receiver->getMethodsCount(); i++)
		{
			thread->createInteger(index, i);
			Class::getMethod(thread, receiver, index, pair);

			if(!pair.isNull())
			{
				Pair::getFirst(thread, pair, otherSelector);
			
				if(selector->compare(*otherSelector) == 0)
				{
					Pair::getSecond(thread, pair, ret);
					frame->stackMoveTop(-3); // pop otherSelector, pair, index
					return; // found!
				}
			}
		}

		frame->stackMoveTop(-3); // pop otherSelector, pair, index
	}

	// try parents
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Class> parent(frame, frame->stackPush());

		for(uint16 i = 0; i < receiver->getParentsCount(); i++)
		{
			thread->createInteger(index, i);
			Class::getParent(thread, receiver, index, parent);

			if(*parent == *receiver)
			{
				continue;
			}

			Class::findMethod(thread, parent, selector, ret);

			if(!ret.isNull())
			{
				break; // found!
			}
		}

		frame->stackMoveTop(-2); // pop index, parent
	}
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

		for(uint16 i = 0; i < receiver->getParentsCount(); i++)
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
				frame->stackMoveTop(-3); // pop index, parent, substituable
				return;
			}
		}

		frame->stackMoveTop(-3); // pop index, parent, substituable
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
	DBG_ASSERT(index->getData() < receiver->getParentsCount(), BEER_WIDEN("Unknown parent"));
	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME + 1 + index->getData()); // 1 for name 
}

void BEER_CALL Class::addParent(Thread* thread, StackRef<Class> receiver, StackRef<Class> value)
{
	DBG_ASSERT(receiver->mParentNext < receiver->getParentsCount(), BEER_WIDEN("Unable to add more parents"));
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// set child
	Object::setChild(thread, receiver, value, CHILD_ID_CLASS_NAME + 1 + (receiver->mParentNext++)); // 1 for name

	// copy properties
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Property> prop(frame, frame->stackPush());

		for(uint16 i = 0; i < value->getPropertiesCount(); i++)
		{
			thread->createInteger(index, i);
			Class::getProperty(thread, value, index, prop);
			Class::addProperty(thread, receiver, prop);
		}

		frame->stackMoveTop(-2); // pop prop, index
	}
}

void BEER_CALL Class::getMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Pair> ret)
{
	DBG_ASSERT(index->getData() < receiver->getMethodsCount(), BEER_WIDEN("Unknown method"));
	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME + 1 + receiver->getParentsCount() + index->getData()); // 1 for name 
}

void BEER_CALL Class::addMethod(Thread* thread, StackRef<Class> receiver, StackRef<Pair> value)
{
	DBG_ASSERT(receiver->mMethodNext < receiver->getMethodsCount(), BEER_WIDEN("Unable to add more methods"));
	Object::setChild(thread, receiver, value, CHILD_ID_CLASS_NAME + 1 + receiver->getParentsCount() + (receiver->mMethodNext++)); // 1 for name
}

void BEER_CALL Class::getProperty(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Property> ret)
{
	DBG_ASSERT(index->getData() < receiver->getPropertiesCount(), BEER_WIDEN("Unknown property"));
	Object::getChild(thread, receiver, ret, CHILD_ID_CLASS_NAME + 1 + receiver->getParentsCount() + receiver->getMethodsCount() + index->getData()); // 1 for name 
}

void BEER_CALL Class::addProperty(Thread* thread, StackRef<Class> receiver, StackRef<Property> value)
{
	DBG_ASSERT(receiver->mPropertyNext < receiver->getPropertiesCount(), BEER_WIDEN("Unable to add more properties")); 
	Object::setChild(thread, receiver, value, CHILD_ID_CLASS_NAME + 1 + receiver->getParentsCount() + receiver->getMethodsCount() + (receiver->mPropertyNext++)); // 1 for name
}