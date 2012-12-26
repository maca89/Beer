#include "stdafx.h"
#include "ClassReflection.h"
#include "ObjectClass.h"
#include "MethodReflection.h"
#include "GarbageCollector.h"
#include "PropertyReflection.h"

using namespace Beer;


Object* ClassReflection::cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
{
	DBG_ASSERT(!object->isInlineValue(), "Cannot clone an inline value");

	Object* copy = createInstance(frame, gc);
	for(uint32 i = 0; i < getPropertiesCount(); i++)
	{
		copy->setChild(i, object->getChild(i));
	}
	return copy;
}

Object* ClassReflection::cloneDeep(Object* object, StackFrame* frame, GarbageCollector* gc)
{
	throw Exception("Not yet implemented", __FILE__, __LINE__);
}


void ClassReflection::extends(uint16 i, ClassReflection* klass)
{
	DBG_ASSERT(i < mParentsCount, "Unable to add more parents");

	// copy parent
	mParents[i] = klass;

	// copy properties
	for(uint16 i = 0; i < klass->mPropertiesCount; i++)
	{
		setProperty(i, klass->mProperties[i]);
	}

	// copy methods
	/*for(uint16 i = 0; i < klass->mMethodsCount; i++)
	{
		setMethod(i, klass->mMethods[i]);
	}*/
}

MethodReflection* ClassReflection::findMethod(const char* selector)
{
	for(uint16 i = 0; i < mMethodsCount; i++)
	{
		if(mMethods[i] && strcmp(mMethods[i]->getSelector(), selector) == 0) return mMethods[i];
	}

	return NULL;
}

bool ClassReflection::substituable(ClassReflection* otherClass) const
{
	for(uint16 i = 0; i < mParentsCount; i++)
	{
		if(mParents[i] == otherClass) return true;
	}
			
	for(uint16 i = 0; i < mParentsCount; i++)
	{
		if(mParents[i]->substituable(otherClass)) return true;
	}

	return false;
}