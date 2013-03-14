#pragma once
#include "prereq.h"
#include "Object.h"
#include "VirtualMachine.h"


namespace Beer
{
	class VirtualMachine;
	class Frame;

	class Task : public Object
	{
	public:
		static void BEER_CALL init(Thread* thread, StackRef<Task> receiver, StackRef<Task> ret);

		static void BEER_CALL schedule(Thread* thread, StackRef<Task> receiver);
		static void BEER_CALL await(Thread* thread, StackRef<Task> receiver);

		static void BEER_CALL getCompleted(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getCanceled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getFailed(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getId(Thread* thread, StackRef<Task> receiver, StackRef<Integer> ret);
	};

	class TaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual void createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret);
		virtual void initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass);
	};
};
