#include "stdafx.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


void BEER_CALL Property::getPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_PROPERTY_TYPE);
}

void BEER_CALL Property::setPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> type)
{
	Object::setChild(thread, receiver, type, CHILD_ID_PROPERTY_TYPE);
}

void BEER_CALL Property::getName(Thread* thread, StackRef<Property> receiver, StackRef<String> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_PROPERTY_NAME);
}

void BEER_CALL Property::setName(Thread* thread, StackRef<Property> receiver, StackRef<String> type)
{
	Object::setChild(thread, receiver, type, CHILD_ID_PROPERTY_NAME);
}