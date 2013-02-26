#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"

using namespace Beer;


void BEER_CALL Task::dorun(Thread* thread, StackFrame* frame, StackRef<Object> receiver)
{
	TrampolineThread* thread2 = new TrampolineThread(thread->getVM());
	thread->getVM()->getThreads().insert(thread2);

	thread2->openStackFrame()->stackPush(receiver.get()); // push receiver

	String* selector = thread2->getVM()->createString(BEER_WIDEN("Task::run()")); // TODO: Task
	Method* method = receiver->getClass<Class>()->findMethod(selector); // TODO
	NULL_ASSERT(method); // TODO
	thread2->openStackFrame()->method = method;

	thread2->run();
	thread2->wait(); // ugly
}