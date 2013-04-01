#pragma once
#include "prereq.h"
#include "Task.h"


namespace Beer
{
	class CreateAllEntryPointsTask : public Task
	{
	public:
		enum
		{
			CREATEEPSTASK_CHILDREN_COUNT = TASK_CHILDREN_COUNT,
			CREATEEPSTASK_METHODS_COUNT = TASK_METHODS_COUNT + 2,
		};


	public:
		static void BEER_CALL init(Thread* thread, StackRef<CreateAllEntryPointsTask> receiver, StackRef<CreateAllEntryPointsTask> ret);
		static void BEER_CALL run(Thread* thread, StackRef<CreateAllEntryPointsTask> receiver);
	};

	class CreateAllEntryPointsTaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};