#pragma once
#include "prereq.h"
#include "Object.h"
#include "VirtualMachine.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;

	class Task
	{
	public:
		static void BEER_CALL init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret);

		static void BEER_CALL start(Thread* thread, StackRef<Object> receiver);
		static void BEER_CALL wait(Thread* thread, StackRef<Object> receiver);
	};

	class TaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
