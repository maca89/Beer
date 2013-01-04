#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassReflection.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
//#include "ClassTable.h"


namespace Beer
{
	class VirtualMachine;

	class ObjectClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			Object* obj = gc->alloc<Object>();
			obj->setClass(this);
			return obj;
		}
	};

	class ObjectClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};