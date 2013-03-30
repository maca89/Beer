#pragma once
#include "prereq.h"
#include "Task.h"


namespace Beer
{
	class InitVMTask : public Task
	{
	public:
		enum
		{
			INITVMTASK_CHILDREN_COUNT = TASK_CHILDREN_COUNT,
			INITVMTASK_METHODS_COUNT = TASK_METHODS_COUNT + 2,
		};

	public:
		static void BEER_CALL init(Thread* thread, StackRef<InitVMTask> receiver, StackRef<InitVMTask> ret);
		static void BEER_CALL work(Thread* thread, StackRef<InitVMTask> receiver);
	};

	class InitVMTaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};