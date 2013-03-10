#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"

using namespace Beer;


void BEER_CALL Task::init(Thread* thread, StackRef<Object> receiver, StackRef<Object> ret)
{
	ret = receiver;
}


void BEER_CALL Task::start(Thread* thread, StackRef<Object> receiver)
{
	TrampolineThread* thread2 = new TrampolineThread(thread->getVM(), thread->getGC());

	thread->getVM()->getThreads().insert(thread2);

	// push receiver
	StackFrame* frame = thread2->openStackFrame();
	frame->stackPush(*receiver); // push receiver

	// push method
	StackRef<Method> method(frame, frame->stackPush());

	{
		// push selector
		StackRef<String> selector(frame, frame->stackPush());
		thread2->createString(selector, string(BEER_WIDEN("Task::run()")));

		// push class
		StackRef<Class> klass(frame, frame->stackPush(
			thread->getClass(receiver)
		));

		// find method
		Class::findMethod(thread2, klass, selector, method);
		frame->stackMoveTop(-2); // pop class, selector
	}

	NULL_ASSERT(*method); // TODO

	Method* m = *method; // ugly, TODO
	frame->stackMoveTop(-1); // pop method
	thread2->openStackFrame()->method = m; // ugly, TODO

	thread2->run();
}


void BEER_CALL Task::wait(Thread* thread, StackRef<Object> receiver)
{
}

Class* TaskInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 3);
}

void TaskInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	loader->extendClass(klass, vm->getObjectClass());
	
	loader->addMethod(klass, BEER_WIDEN("Task"), BEER_WIDEN("Task::Task()"), &Task::init, 1, 0);
	
	loader->addMethod(klass, BEER_WIDEN("start"), BEER_WIDEN("Task::start()"), &Task::start, 1, 0);
	// TODO: abstract run
	loader->addMethod(klass, BEER_WIDEN("wait"), BEER_WIDEN("Task::wait()"), &Task::wait, 1, 0);
}