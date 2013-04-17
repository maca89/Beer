#include "stdafx.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


Class* Property::getPropertyType()
{
	return static_cast<Class*>(getChild(CHILD_ID_PROPERTY_TYPE));
}

void Property::setPropertyType(Class* value)
{
	setChild(CHILD_ID_PROPERTY_TYPE, value);
}

String* Property::getPropertyName()
{
	return static_cast<String*>(getChild(CHILD_ID_PROPERTY_NAME));
}

void Property::setPropertyName(String* value)
{
	setChild(CHILD_ID_PROPERTY_NAME, value);
}

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