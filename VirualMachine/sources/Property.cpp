#include "stdafx.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


void BEER_CALL Property::getPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> ret)
{
	ret = receiver->getPropertyType();
}

void BEER_CALL Property::setPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> value)
{
	receiver->setPropertyType(*value);
}

void BEER_CALL Property::getPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> ret)
{
	ret = receiver->getPropertyName();
}

void BEER_CALL Property::setPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> value)
{
	receiver->setPropertyName(*value);
}

void Property::PropertyTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* inst)
{
	Class::DefaultInstanceTraverser(receiver, klass, inst);

	Property* instance = static_cast<Property*>(inst);

	receiver->traverseObjectPtr(reinterpret_cast<Object**>(&instance->mPropertyType));
	receiver->traverseObjectPtr(reinterpret_cast<Object**>(&instance->mPropertyName));

	for(uint32 i = OBJECT_CHILDREN_COUNT; i < Property::PROPERTY_CHILDREN_COUNT; i++)
	{
		receiver->traverseObjectPtr(&instance->getChildren()[i]);
	}
}