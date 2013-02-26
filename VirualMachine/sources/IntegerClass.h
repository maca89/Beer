#pragma once
#include "prereq.h"
#include "Integer.h"
#include "Class.h"
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
		static void BEER_CALL createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<Integer> ret);
	};
	
	class IntegerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};