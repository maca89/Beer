#include "stdafx.h"
#include "ChildrenForwarder.h"

#include "Object.h"
#include "GCObject.h"
#include "Class.h"

using namespace Beer;

void ChildrenForwarder::forward(GCObject* gcObj)
{
	DBG_ASSERT(!gcObj->getPtr(), BEER_WIDEN("forwarded object can not have forwarding pointer"));

	Object* obj = gcObj->getObject();
	Class* type = obj->getType();

	DBG_ASSERT(type != NULL, BEER_WIDEN("Type of object is NULL"));

	type->getTraverser()(this, type, obj);
}

void ChildrenForwarder::traverseObjectPtr(Object** ptrToObject)
{
	Object* obj = *ptrToObject;

	if (obj != NULL && !Object::isInlineValue(obj)) // Checking if pointer points to nursery heap is not necessary, but it might be faster than forwarding every pointer
	{
		*ptrToObject = GCObject::get(obj)->forward();
	}
}