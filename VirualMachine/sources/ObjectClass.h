#pragma once
#include "prereq.h"
#include "Object.h"
#include "Class.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
//#include "ClassTable.h"


namespace Beer
{
	class VirtualMachine;

	class ObjectClass : public Class
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			Object* object = gc->alloc<Object>(Object::OBJECT_CHILDREN_COUNT);
			
			object->setClass(this);
			//invoke(object, BEER_WIDEN("Object::setClass(Class)"));

			//obj->setClass(this);
			return object;
		}
	};
};