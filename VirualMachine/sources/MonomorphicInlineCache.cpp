#include "stdafx.h"
#include "MonomorphicInlineCache.h"
#include "StackFrame.h"
#include "Thread.h"
#include "Class.h"

using namespace Beer;


void MonomorphicInlineCache::lookup(Thread* thread, Class* klass, String* selector)
{
	StackFrame* frame = thread->getStackFrame();
	StackRef<Class> klassOnStack(frame, frame->stackPush(klass));
	StackRef<String> selectorOnStack(frame, frame->stackPush(selector));
	StackRef<Method> methodOnStack(frame, frame->stackPush());

	Class::findMethod(thread, klassOnStack, selectorOnStack, methodOnStack);
	mMethod = *methodOnStack;

	frame->stackMoveTop(-3); // pop class, selector, method
}