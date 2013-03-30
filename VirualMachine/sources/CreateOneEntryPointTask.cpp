#include "stdafx.h"
#include "CreateOneEntryPointTask.h"
#include "TrampolineThread.h"
#include "Method.h"

using namespace Beer;


void BEER_CALL CreateOneEntryPointTask::init_Class(Thread* thread, StackRef<CreateOneEntryPointTask> receiver, StackRef<Class> klass, StackRef<CreateOneEntryPointTask> ret)
{
	Object::setChild(thread, receiver, CHILD_ID_ENTRY_POINT_CLASS, klass);
	ret = receiver;
}

void BEER_CALL CreateOneEntryPointTask::work(Thread* thread, StackRef<CreateOneEntryPointTask> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush());
	Object::getChild(thread, receiver, CHILD_ID_ENTRY_POINT_CLASS, klass);

	StackRef<Object> instance(frame, frame->stackPush(NULL));

	// create new instance
	thread->createInstance(klass, instance);


	// call constuctor
#ifdef BEER_FOLDING_BLOCK
	if(true){
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

		StackRef<Method> method(frame, frame->stackPush());
		Class::findMethod(thread, klass, selector, method);

		if(!method.isNull())
		{
			// ugly
			TrampolineThread thread2(thread->getVM(), thread->getGC());

			thread2.getFrame()->stackPush(); // for return
			thread2.getFrame()->stackPush(*instance); // push receiver
			thread2.getFrame()->stackPush(*method); // push method
			thread2.openFrame();

			thread2.trampoline();
			//instance = thread->getFrame()->stackTop(); // fix main // TODO
		}

		frame->stackMoveTop(-2); // pop method & selector
	}
#endif // BEER_FOLDING_BLOCK


	// Task::run()
	{
#ifdef BEER_FOLDING_BLOCK
		StackRef<Method> method(frame, frame->stackPush());
		//getDebugger()->printFrameStack(frame);
				
		// fetch method
		{
#ifdef BEER_FOLDING_BLOCK
			StackRef<String> selector(frame, frame->stackPush()); // TODO
			thread->createString(selector, BEER_WIDEN("Task::schedule()")); // why the cast?!, TODO

			Class::findMethod(thread, klass, selector, method);

			if(method.isNull())
			{
				throw MethodNotFoundException(*instance, *klass, *selector); // TODO
			}
			frame->stackPop(); // pop selector
#endif // BEER_FOLDING_BLOCK
		}

		//getDebugger()->printFrameStack(frame);

		thread->openFrame();
		method->invoke(thread); // closes frame, pop method, instance
#endif // BEER_FOLDING_BLOCK
	}

	//frame->stackPop(); // pop instance
	frame->stackPop(); // pop klass
}

Class* CreateOneEntryPointTaskInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, CreateOneEntryPointTask::CREATEEPTASK_CHILDREN_COUNT, CreateOneEntryPointTask::CREATEEPTASK_METHODS_COUNT);
}

void CreateOneEntryPointTaskInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->addParent(thread->getVM()->findClass(BEER_WIDEN("Task")));
	
	loader->addMethod(thread, klass, BEER_WIDEN("CreateOneEntryPointTask"), BEER_WIDEN("CreateOneEntryPointTask::CreateOneEntryPointTask(Class)"), &CreateOneEntryPointTask::init_Class, 1, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("work"), BEER_WIDEN("Task::work()"), &CreateOneEntryPointTask::work, 0, 0); // interface method, TODO: second selector
}