#pragma once
#include "prereq.h"
#include "Class.h"
#include "GenerationalGC.h"
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
		
		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Timer> ret);

		static void BEER_CALL init(Thread* thread, StackRef<Timer> receiver, StackRef<Timer> ret);
		static void BEER_CALL start(Thread* thread, StackRef<Timer> receiver);
		static void BEER_CALL stop(Thread* thread, StackRef<Timer> receiver, StackRef<Float> ret);	
	};

	class TimerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};
