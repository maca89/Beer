#include "stdafx.h"
#include "Task.h"
#include "MethodReflection.h"
#include "VirtualMachine.h"
#include "LoadedObject.h"

using namespace Beer;


void BEER_CALL Task::dorun(VirtualMachine* vm, StackFrame* frame, StackRef<Object> receiver)
{
	TrampolineThread* thread = new TrampolineThread(vm);
	vm->getThreads().insert(thread);

	thread->openStackFrame()->stackPush(receiver.get()); // push receiver

	MethodReflection* method = receiver->getClass<Class>()->findMethod(BEER_WIDEN("Main::run()")); // ugly, TODO: Task::run
	NULL_ASSERT(method); // TODO
	thread->openStackFrame()->method = method;

	thread->run();
	thread->wait(); // ugly
}