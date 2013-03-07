#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"

using namespace Beer;


void BEER_CALL Task::dorun(Thread* thread, StackRef<Object> receiver)
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
			receiver->getClass<Class>()
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