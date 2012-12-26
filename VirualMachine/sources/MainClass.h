#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"

namespace Beer
{
	class VirtualMachine;

	class Main : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();

	protected:

	public:
		
	};

	class MainClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			Main* cons = gc->alloc<Main>();
			cons->setClass(this);
			return cons;
		}
	};

	class MainClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};