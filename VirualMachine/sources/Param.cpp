#include "stdafx.h"
#include "Param.h"
#include "Class.h"

using namespace Beer;


Class* Param::getType()
{
	return getChild<Class>(OBJECT_CHILDREN_COUNT);
}

void Param::setType(Class* value)
{
	setChild(OBJECT_CHILDREN_COUNT, value);
}

String* Param::getName()
{
	return getChild<String>(OBJECT_CHILDREN_COUNT + 1);
}

void Param::setName(String* value)
{
	setChild(OBJECT_CHILDREN_COUNT + 1, value);
}