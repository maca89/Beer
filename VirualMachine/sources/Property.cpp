#include "stdafx.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


Class* Property::getType()
{
	return _getChild<Class>(OBJECT_CHILDREN_COUNT);
}

void Property::setType(Class* value)
{
	_setChild(OBJECT_CHILDREN_COUNT, value);
}

String* Property::getName()
{
	return _getChild<String>(OBJECT_CHILDREN_COUNT + 1);
}

void Property::setName(String* value)
{
	_setChild(OBJECT_CHILDREN_COUNT + 1, value);
}