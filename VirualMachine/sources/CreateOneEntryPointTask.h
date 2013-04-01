#pragma once
#include "prereq.h"
#include "Task.h"


namespace Beer
{
	class CreateOneEntryPointTask : public Task
	{
	public:
		enum
		{
			CREATEEPTASK_CHILDREN_COUNT = TASK_CHILDREN_COUNT + 1, // +1 for instance of EntryPoint
			CREATEEPTASK_METHODS_COUNT = TASK_METHODS_COUNT + 2,

			CHILD_ID_ENTRY_POINT_CLASS = TASK_CHILDREN_COUNT,
		};


	public:
		static void BEER_CALL init_Class(Thread* thread, StackRef<CreateOneEntryPointTask> receiver, StackRef<Class> klass, StackRef<CreateOneEntryPointTask> ret);
		static void BEER_CALL run(Thread* thread, StackRef<CreateOneEntryPointTask> receiver);
	};

	class CreateOneEntryPointTaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};