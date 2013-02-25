#include "stdafx.h"
#include "Task.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"

using namespace Beer;


void BEER_CALL Task::dorun(VirtualMachine* vm, StackFrame* frame, StackRef<Object> receiver)
{
	TrampolineThread* thread = new TrampolineThread(vm);
	vm->getThreads().insert(thread);

	thread->openStackFrame()->stackPush(receiver.get()); // push receiver

	String* selector = vm->createString(BEER_WIDEN("Task::run()")); // TODO: Task
	Method* method = receiver->getClass<Class>()->findMethod(selector); // TODO
	NULL_ASSERT(method); // TODO
	thread->openStackFrame()->method = method;

	thread->run();
	thread->wait(); // ugly
}