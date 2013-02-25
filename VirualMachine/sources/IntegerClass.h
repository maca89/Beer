#pragma once
#include "prereq.h"
#include "Integer.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
//#include "ClassTable.h"


namespace Beer
{
	class VirtualMachine;

	class IntegerClass : public Class
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			Integer* num = gc->alloc<Integer>(Object::OBJECT_CHILDREN_COUNT);
			num->setClass(this);
			return num;
		}
	};
	
	class IntegerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};