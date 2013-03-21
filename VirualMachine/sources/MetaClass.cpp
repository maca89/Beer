#include "stdafx.h"
#include "MetaClass.h"
#include "VirtualMachine.h"

using namespace Beer;


void BEER_CALL MetaClass::init(Thread* thread, StackRef<MetaClass> receiver, StackRef<MetaClass> ret)
{
	ret = receiver;
}

/*void BEER_CALL MetaClass::findMethod(Thread* thread, StackRef<MetaClass> receiver, StackRef<String> selector, StackRef<Method> ret)
{
	// TODO
}*/

void MetaClass::DefaultClassTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* inst)
{
	Class* instance = static_cast<Class*>(inst);
	uint32 childrenCount = Class::CLASS_CHILDREN_COUNT + instance->getParentsCount() + instance->getMethodsCount() + instance->getPropertiesCount();

	for(uint32 i = 0; i < childrenCount; i++)
	{
		//receiver->traverseObjectPtr(&instance->getChildren()[i]);
	}
}