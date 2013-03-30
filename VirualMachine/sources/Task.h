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
		enum
		{
			TASK_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT,
			TASK_METHODS_COUNT = OBJECT_METHODS_COUNT + 8,
		};


	public:
		static void BEER_CALL init(Thread* thread, StackRef<Task> receiver, StackRef<Task> ret);

		static void BEER_CALL schedule(Thread* thread, StackRef<Task> receiver);
		static void BEER_CALL await(Thread* thread, StackRef<Task> receiver);

		static void BEER_CALL getCompleted(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getCanceled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getFailed(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getId(Thread* thread, StackRef<Task> receiver, StackRef<Integer> ret);

		static void BEER_CALL abstractWork(Thread* thread, StackRef<Task> receiver);
	};

	class TaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};
