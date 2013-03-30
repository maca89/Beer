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

	frame->stackPop(); // pop propertiesCount
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

	frame->stackPop(); // pop index
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
	Object::getChild(thread, receiver, CHILD_ID_CLASS_NAME, ret);
}

void BEER_CALL Class::setName(Thread* thread, StackRef<Class> receiver, StackRef<String> value)
{
	Object::setChild(thread, receiver, CHILD_ID_CLASS_NAME, value);
}

void Class::addParent(Class* klass)
{
	DBG_ASSERT(hasParentFreeSlot(), BEER_WIDEN("Unable to add more parents"));

	// set child
	setParent(mParentNext++, klass);

	// copy methods
	if(this != klass)
	{
		for(uint16 i = 0; i < klass->getMethodsCount(); i++)
		{
			Pair* method = klass->getMethod(i);
			addMethod(method);
		}
	}

	// copy properties
	{
		for(uint16 i = 0; i < klass->getPropertiesCount(); i++)
		{
			Property* prop = klass->getProperty(i);
			addProperty(prop);
		}
	}
}

void BEER_CALL Class::getParent(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Class> ret)
{
	ret = receiver->getParent(static_cast<uint32>(index->getData()));
}

void BEER_CALL Class::getParentsCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mParentsCount); // TODO
}

void BEER_CALL Class::addParent(Thread* thread, StackRef<Class> receiver, StackRef<Class> value)
{
	receiver->addParent(*value);
}

void Class::addMethod(Pair* value)
{
	DBG_ASSERT(hasMethodFreeSlot(), BEER_WIDEN("Unable to add more methods"));
	setMethod(mMethodNext++, value);
}

void BEER_CALL Class::getMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Pair> ret)
{
	ret = receiver->getMethod(static_cast<uint32>(index->getData()));
}

/*void Class::getOnlyMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Method> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Pair> pair(frame, frame->stackPush());
	Class::getMethod(thread, receiver, index, pair);

	Pair::getSecond(thread, pair, ret);

	frame->stackPop(); // pop pair
}*/

void BEER_CALL Class::getMethodsCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mMethodsCount); // TODO
}

void BEER_CALL Class::addMethod(Thread* thread, StackRef<Class> receiver, StackRef<Pair> value)
{
	receiver->addMethod(*value);
}

void Class::addProperty(Property* value)
{
	DBG_ASSERT(hasPropertyFreeSlot(), BEER_WIDEN("Unable to add more properties"));
	setProperty(mPropertyNext++, value);
}

void Class::getProperty(Thread* thread, StackRef<Class> receiver, uint32 index, StackRef<Property> ret)
{
	ret = receiver->getProperty(index);
}

void BEER_CALL Class::getProperty(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Property> ret)
{
	ret = receiver->getProperty(static_cast<uint32>(index->getData()));
}

void BEER_CALL Class::getPropertiesCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret)
{
	thread->createInteger(ret, receiver->mPropertiesCount); // TODO
}

void BEER_CALL Class::addProperty(Thread* thread, StackRef<Class> receiver, StackRef<Property> value)
{
	receiver->addProperty(*value);
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

void Class::DefaultInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance)
{
	if(!Object::isInlineValue(instance))
	{
		receiver->traverseObjectPtr(reinterpret_cast<Object**>(&instance->mType));
	
		uint32 childrenCount = Object::OBJECT_CHILDREN_COUNT + klass->getPropertiesCount();

		for(uint32 i = 0; i < childrenCount; i++)
		{
			receiver->traverseObjectPtr(&instance->getChildren()[i]);
		}
	}
}