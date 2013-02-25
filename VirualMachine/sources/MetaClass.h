#pragma once
#include "prereq.h"
#include "Class.h"
#include "ClassLoader.h"
#include "GarbageCollector.h"
#include "VirtualMachine.h"


namespace Beer
{
	// TODO
	class MetaClass : public Class
	{
	public:
		// Class
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			Class* object = gc->alloc<Class>(Class::CLASS_CHILDREN_COUNT);
			
			object->setClass(this);
			//invoke(object, BEER_WIDEN("Object::setClass(Class)"));

			//obj->setClass(this);
			return object;
		}

		static void BEER_CALL init(VirtualMachine* vm, StackFrame* frame, StackRef<MetaClass> receiver, StackRef<MetaClass> ret);
		static void BEER_CALL findMethod(VirtualMachine* vm, StackFrame* frame, StackRef<MetaClass> receiver, StackRef<String> selector, StackRef<Method> ret);
	};
};