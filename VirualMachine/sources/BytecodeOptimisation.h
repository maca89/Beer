#pragma once
#include "prereq.h"

namespace Beer
{
	class Bytecode;

	class BytecodeOptimisation
	{
	public:
		virtual ~BytecodeOptimisation() {}

		virtual bool optimiseRaw(byte* data, uint32 dataLength, uint16 instrCount) = 0;
		//virtual void optimiseBuild(Bytecode* bc, ClassFileDescriptor* classFile) = 0;
	};
};