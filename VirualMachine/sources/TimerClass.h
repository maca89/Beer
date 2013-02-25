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

	class TimerClass : public Class
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			// TODO: constructor
			Timer* timer = gc->alloc<Timer>(Object::OBJECT_CHILDREN_COUNT);
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
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
