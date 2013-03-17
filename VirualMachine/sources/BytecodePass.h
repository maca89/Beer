#pragma once
#include "prereq.h"

namespace Beer
{
	class Bytecode;

	class BytecodePass
	{
	public:
		virtual ~BytecodePass() {}

		virtual void process(byte* data, uint32 dataLength, uint16 instrCount) = 0;
	};

	class BytecodeLoader : public BytecodePass
	{

	};

	class BytecodeLinker : public BytecodePass
	{

	};

	class BytecodeVerifier : public BytecodePass
	{

	};

	class BytecodeOptimiser : public BytecodePass
	{

	};

	class BytecodeExecuter : public BytecodePass
	{

	};

	class BytecodeOptimisation
	{
	public:
		virtual ~BytecodeOptimisation() {}

		virtual bool optimiseRaw(byte* data, uint32 dataLength, uint16 instrCount) = 0;
		//virtual void optimiseBuild(Bytecode* bc, ClassFileDescriptor* classFile) = 0;
	};
};