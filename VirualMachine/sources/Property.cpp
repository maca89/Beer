#include "stdafx.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


Class* Property::getType()
{
	return getChild<Class>(OBJECT_CHILDREN_COUNT);
}

void Property::setType(Class* value)
{
	setChild(OBJECT_CHILDREN_COUNT, value);
}

String* Property::getName()
{
	return getChild<String>(OBJECT_CHILDREN_COUNT + 1);
}

void Property::setName(String* value)
{
	setChild(OBJECT_CHILDREN_COUNT + 1, value);
}