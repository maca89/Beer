#pragma once
#include "prereq.h"
#include "Class.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "MiliTimer.h"
#include "Float.h"


namespace Beer
{
	class VirtualMachine;

	class Timer : public Object
	{
	protected:
		MiliTimer mTimer;

	public:
		
		static void BEER_CALL createInstance(Thread* thread, StackFrame* frame, StackRef<Class> receiver, StackRef<Timer> ret);

		static void BEER_CALL init(Thread* thread, StackFrame* frame, StackRef<Timer> receiver, StackRef<Timer> ret);
		static void BEER_CALL start(Thread* thread, StackFrame* frame, StackRef<Timer> receiver);
		static void BEER_CALL stop(Thread* thread, StackFrame* frame, StackRef<Timer> receiver, StackRef<Float> ret);	
	};

	class TimerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
