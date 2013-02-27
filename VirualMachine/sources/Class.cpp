#include "stdafx.h"
#include "Class.h"
#include "Object.h"
#include "Method.h"
#include "GarbageCollector.h"
#include "Property.h"
#include "String.h"
#include "VirtualMachine.h"

using namespace Beer;


/*Object* Class::cloneShallow(VirtualMachine* vm, Object* object, StackFrame* frame, GarbageCollector* gc)
{
	DBG_ASSERT(!isInlineValue(object), BEER_WIDEN("Cannot clone an inline value"));

	Object* copy = createInstance(vm, frame, gc);
	for(uint32 i = 0; i < getPropertiesCount(); i++)
	{
		copy->setChild(i, object->getChild(i));
	}
	return copy;
}

Object* ClassReflection::cloneDeep(VirtualMachine* vm, Object* object, StackFrame* frame, GarbageCollector* gc)
{
	throw Exception(BEER_WIDEN("Not yet implemented"), __WFILE__, __LINE__);
}*/

void Class::extends(uint16 i, Class* klass)
{
	DBG_ASSERT(i < mParentsCount, BEER_WIDEN("Unable to add more parents"));

	// copy parent
	setParent(i, klass);

	// copy properties
	for(uint16 i = 0; i < klass->getPropertiesCount(); i++)
	{
		setProperty(i, klass->getProperty(i));
	}

	// copy methods
	/*for(uint16 i = 0; i < klass->mMethodsCount; i++)
	{
		setMethod(i, klass->mMethods[i]);
	}*/
}

Method* Class::findMethod(String* selector)
{
	for(uint16 i = 0; i < mMethodsCount; i++)
	{
		Pair* method = getMethod(i);
		if(method)
		{
			String* s2 = method->getFirst<String>();
			//Object* o = method->getFirst<Object>();
			if(selector->compare(s2) == 0)
			{
				return method->getSecond<Method>();
			}
		}
	}

	for(uint16 i = 0; i < mParentsCount; i++)
	{
		Class* parent = getParent(i);
		if(parent == this) continue;
		Method* method = parent->findMethod(selector);
		if(method) return method;
	}

	return NULL;
}

void BEER_CALL Class::createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<Object> ret)
{
	Class* klass = *receiver;

	ret = ((GarbageCollector*)thread->getHeap())->alloc<Object>(
		Object::OBJECT_CHILDREN_COUNT + receiver->getPropertiesCount()
	);
			
	ret->setClass(*receiver); // TODO
	//invoke(object, BEER_WIDEN("Object::setClass(Class)"));
}

bool Class::substituable(Class* otherClass) const
{
	for(uint16 i = 0; i < mParentsCount; i++)
	{
		if(getParent(i) == otherClass) return true;
	}
			
	for(uint16 i = 0; i < mParentsCount; i++)
	{
		if(getParent(i)->substituable(otherClass)) return true;
	}

	return false;
}