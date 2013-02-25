#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "VirtualMachine.h"


namespace Beer
{
	class VirtualMachine;
	class StackFrame;

	class Task
	{
	public:
		static void BEER_CALL dorun(VirtualMachine* vm, StackFrame* frame, StackRef<Object> receiver);
	};
};
