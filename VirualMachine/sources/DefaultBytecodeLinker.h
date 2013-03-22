#pragma once
#include "prereq.h"
#include "BytecodePass.h"


namespace Beer
{
	class ClassFileDescriptor;
	class Thread;
	class Bytecode;

	class DefaultBytecodeLinker : public BytecodeLinker
	{
	public:
		virtual void link(Thread* thread, StackRef<Method> method, ClassFileDescriptor* classFile, const TemporaryBytecode& bc, TemporaryBytecode& out_bc);
	};
};