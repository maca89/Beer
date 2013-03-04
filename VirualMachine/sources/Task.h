#pragma once
#include "prereq.h"
#include "Object.h"
#include "GarbageCollector.h"
#include "VirtualMachine.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;

	class Task
	{
	public:
		static void BEER_CALL dorun(Thread* thread, StackFrame* frame, StackRef<Object> receiver);
	};
};
