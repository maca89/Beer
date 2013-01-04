#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "TrampolineThread.h"


namespace Beer
{
	class VirtualMachine;
	class TaskClass;
	//class Thread;

	// *NO* -- its an interface!
	/*class Task : public Object
	{
	protected:
		Thread* mThread;
		//friend class TaskClass;

	public:
		INLINE void setThread(Thread* value) { mThread = value; }
		INLINE Thread* getThread() const { return mThread; }

		void start();
	};*/

	class TaskClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			throw Exception(BEER_WIDEN("Tried to create instance of interface Task"), __WFILE__, __LINE__);
			/*Task* object = gc->alloc<Task>();
			object->setClass(this);
			object->setThread(new TrampolineThread(vm)); // TODO: call desctuctor
			return object;*/
		}
	};

	class TaskClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};
