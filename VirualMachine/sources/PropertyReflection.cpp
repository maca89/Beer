#include "stdafx.h"
#include "PropertyReflection.h"
#include "Class.h"

using namespace Beer;


Class* PropertyReflection::getType()
{
	return getChild<Class>(OBJECT_CHILDREN_COUNT);
}

void PropertyReflection::setType(Class* value)
{
	setChild(OBJECT_CHILDREN_COUNT, value);
}

String* PropertyReflection::getName()
{
	return getChild<String>(OBJECT_CHILDREN_COUNT + 1);
}

void PropertyReflection::setName(String* value)
{
	setChild(OBJECT_CHILDREN_COUNT + 1, value);
}