#include "stdafx.h"
#include "Param.h"
#include "Class.h"

using namespace Beer;


void BEER_CALL Param::getParamType(Thread* thread, StackRef<Param> receiver, StackRef<Class> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_PARAM_TYPE);
}

void BEER_CALL Param::setParamType(Thread* thread, StackRef<Param> receiver, StackRef<Class> type)
{
	Object::setChild(thread, receiver, type, CHILD_ID_PARAM_TYPE);
}

void BEER_CALL Param::getName(Thread* thread, StackRef<Param> receiver, StackRef<String> ret)
{
	Object::getChild(thread, receiver, ret, CHILD_ID_PARAM_NAME);
}

void BEER_CALL Param::setName(Thread* thread, StackRef<Param> receiver, StackRef<String> type)
{
	Object::setChild(thread, receiver, type, CHILD_ID_PARAM_NAME);
}