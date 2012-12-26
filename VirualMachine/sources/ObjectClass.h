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

	/*class Object : public CollectedObject
	{
	public:
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();

	protected:
		~Object() // Object and child classes can *NOT* have a destructor
		{
			throw GCException("Object::~Object() called!");
		}

	private:
		Object() // Object and child classes can *NOT* have a constructor
		{
			throw GCException("Object::Object() called!");
		}
	};*/

	class ObjectClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
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
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};