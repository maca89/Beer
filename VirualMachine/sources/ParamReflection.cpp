#include "stdafx.h"
#include "ParamReflection.h"
#include "Class.h"

using namespace Beer;


Class* ParamReflection::getType()
{
	return getChild<Class>(OBJECT_CHILDREN_COUNT);
}

void ParamReflection::setType(Class* value)
{
	setChild(OBJECT_CHILDREN_COUNT, value);
}

String* ParamReflection::getName()
{
	return getChild<String>(OBJECT_CHILDREN_COUNT + 1);
}

void ParamReflection::setName(String* value)
{
	setChild(OBJECT_CHILDREN_COUNT + 1, value);
}