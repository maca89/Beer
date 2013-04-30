#include "stdafx.h"
#include "CreateOneEntryPointTask.h"
#include "TrampolineThread.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Debugger.h"

using namespace Beer;


void BEER_CALL CreateOneEntryPointTask::init_Class(Thread* thread, StackRef<CreateOneEntryPointTask> receiver, StackRef<Class> klass, StackRef<CreateOneEntryPointTask> ret)
{
	Task::init(thread, receiver.staticCast<Task>(), ret.staticCast<Task>());
	Object::setChild(thread, receiver, CHILD_ID_ENTRY_POINT_CLASS, klass);
}

void BEER_CALL CreateOneEntryPointTask::run(Thread* thread, StackRef<CreateOneEntryPointTask> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush());
	Object::getChild(thread, receiver, CHILD_ID_ENTRY_POINT_CLASS, klass);

	StackRef<Task> instance(frame, frame->stackPush(NULL));

	// create new instance
	thread->createInstance(klass, instance);


	// call Main::Main() constuctor
#ifdef BEER_FOLDING_BLOCK
	if(false){
		StackRef<String> selector(frame, frame->stackPush());

		StackRef<String> tmpString1(frame, frame->stackPush());
		thread->createString(tmpString1, BEER_WIDEN("::"));

		StackRef<String> tmpString2(frame, frame->stackPush());
		Class::getName(thread, klass, tmpString2);

		String::operatorAddString(thread, tmpString2, tmpString1, selector); // Main::
		String::operatorAddString(thread, selector, tmpString2, tmpString1); // Main::Main

		thread->createString(tmpString2, BEER_WIDEN("()"));

		String::operatorAddString(thread, tmpString1, tmpString2, selector); // Main::Main()
				
		frame->stackMoveTop(-2); // pop tmpString1, tmpString2

		Method* method = klass->findVirtualMethod(*selector);

		if(method)
		{
			// ugly
			TrampolineThread thread2(42, thread->getVM(), thread->getGC());

			thread2.getFrame()->stackPush(); // for return
			thread2.getFrame()->stackPush(*instance); // push receiver
			thread2.getFrame()->stackPush(method); // push method
			thread2.openFrame();

			thread2.trampoline();
			//instance = thread->getFrame()->stackTop(); // fix main // TODO
		}

		frame->stackPop(); // selector
	}
	// call Task::Task() constructor
	else
	{
		Task::init(thread, instance, instance);
	}
#endif // BEER_FOLDING_BLOCK


	thread->getVM()->getScheduler()->addTask(*instance);

	frame->stackPop(); // pop instance
	frame->stackPop(); // pop klass
}

Class* CreateOneEntryPointTaskInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, CreateOneEntryPointTask::CREATEEPTASK_CHILDREN_COUNT, CreateOneEntryPointTask::CREATEEPTASK_METHODS_COUNT);
}

void CreateOneEntryPointTaskInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->setSuperClass(thread->getVM()->findClass(BEER_WIDEN("Task")));
	klass->markSealed();
	
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("CreateOneEntryPointTask"), BEER_WIDEN("CreateOneEntryPointTask::CreateOneEntryPointTask(Class)"), &CreateOneEntryPointTask::init_Class, 1, 1);
	loader->addOverrideMethod(thread, klass, BEER_WIDEN("run"), BEER_WIDEN("Task::run()"), &CreateOneEntryPointTask::run, 0, 0); // interface method, TODO: second selector
}