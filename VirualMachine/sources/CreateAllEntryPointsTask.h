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
		static void BEER_CALL work(Thread* thread, StackRef<CreateAllEntryPointsTask> receiver);
	};

	class CreateAllEntryPointsTaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual void createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret);
		virtual void initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass);
	};
};