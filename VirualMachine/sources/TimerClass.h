#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "Timer.h"


namespace Beer
{
	class VirtualMachine;

	class Timer : public Object
	{
	protected:
		MiliTimer mTimer;

	public:
		INLINE void start() { mTimer.start(); }
		INLINE float64 stop() { return mTimer.stop(); }

		friend class TimerClass;
	};

	class TimerClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			// TODO: constructor
			Timer* timer = gc->alloc<Timer>();
			timer->setClass(this);
			timer->mTimer = MiliTimer();
			timer->start();
			return timer;
		}
	};

	class TimerClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};
