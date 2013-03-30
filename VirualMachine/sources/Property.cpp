#include "stdafx.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


void BEER_CALL Property::getPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> ret)
{
	Object::getChild(thread, receiver, CHILD_ID_PROPERTY_TYPE, ret);
}

void BEER_CALL Property::setPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> type)
{
	Object::setChild(thread, receiver, CHILD_ID_PROPERTY_TYPE, type);
}

void BEER_CALL Property::getPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> ret)
{
	Object::getChild(thread, receiver, CHILD_ID_PROPERTY_NAME, ret);
}

void BEER_CALL Property::setPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> type)
{
	Object::setChild(thread, receiver, CHILD_ID_PROPERTY_NAME, type);
}