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
			Object* obj = gc->alloc<Object>(Object::OBJECT_CHILDREN_COUNT);
			obj->setClass(this);
			return obj;
		}
	};
};