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
		virtual Bytecode* link(Thread* thread, ClassFileDescriptor* classFile, byte* data, uint32 dataLength, uint16 instrCount);
	};
};