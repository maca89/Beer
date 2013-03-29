#include "stdafx.h"
#include "CreateAllEntryPointsTask.h"
#include "TrampolineThread.h"
#include "Method.h"
#include "CreateOneEntryPointTask.h"

using namespace Beer;


void BEER_CALL CreateAllEntryPointsTask::init(Thread* thread, StackRef<CreateAllEntryPointsTask> receiver, StackRef<CreateAllEntryPointsTask> ret)
{
	ret = receiver;
}

void BEER_CALL CreateAllEntryPointsTask::work(Thread* thread, StackRef<CreateAllEntryPointsTask> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> entryPointClass(frame, frame->stackPush(
		thread->getVM()->findClass(BEER_WIDEN("EntryPoint"))
	));
	
	StackRef<Class> createEntryPointTaskClass(frame, frame->stackPush(
		thread->getVM()->findClass(BEER_WIDEN("CreateOneEntryPointTask"))
	));

	if(entryPointClass.isNull())
	{
		throw ClassNotFoundException(BEER_WIDEN("Class EntryPoint not found"));
	}

	for(ClassReflectionTable::iterator it = thread->getVM()->getClasses().begin(); it != thread->getVM()->getClasses().end(); it++)
	{
		StackRef<Class> klass(frame, frame->stackPush(it->second)); // TODO
		bool substituable = false;

		// fetch substituable
		{
			StackRef<Boolean> tmp(frame, frame->stackPush());
			Class::substituable(thread, klass, entryPointClass, tmp);
			substituable = tmp->getData();
			frame->stackPop(); // pop tmp
		}

		// TODO: check if Task::run is not abstract (eg. for base abstract classes)
		if(klass.get() != entryPointClass.get() && substituable)  // TODO
		{
			StackRef<CreateOneEntryPointTask> task(frame, frame->stackPush());
			thread->createInstance(createEntryPointTaskClass, task);
			
			CreateOneEntryPointTask::init_Class(thread, task, klass, task);
			CreateOneEntryPointTask::work(thread, task);
			// TODO: schedule task

			frame->stackPop(); // pop task
		}

		frame->stackPop(); // pop class
	}

	frame->stackPop(); // pop createEntryPointTaskClass
	frame->stackPop(); // pop entryPointClass
}

void CreateAllEntryPointsTaskInitializer::createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret)
{
	return loader->createClass<Class>(thread, name, ret, 1, CreateAllEntryPointsTask::CREATEEPSTASK_CHILDREN_COUNT, CreateAllEntryPointsTask::CREATEEPSTASK_METHODS_COUNT);
}

void CreateAllEntryPointsTaskInitializer::initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	{
		StackRef<Class> taskClass(frame, frame->stackPush(
			thread->getVM()->findClass(BEER_WIDEN("Task"))
		));
		Class::addParent(thread, klass, taskClass);
		frame->stackPop(); //  pop taskClass
	}
	
	loader->addMethod(thread, klass, BEER_WIDEN("CreateAllEntryPointsTask"), BEER_WIDEN("CreateAllEntryPointsTask::CreateAllEntryPointsTask()"), &CreateAllEntryPointsTask::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("work"), BEER_WIDEN("Task::work()"), &CreateAllEntryPointsTask::work, 0, 0); // interface method, TODO: second selector
}