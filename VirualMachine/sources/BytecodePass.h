#pragma once
#include "prereq.h"
#include "StackRef.h"


namespace Beer
{
	class Thread;
	class Bytecode;
	class BytecodeDescriptor;
	class ClassFileDescriptor;

	class BytecodeLoader
	{
	public:
		virtual ~BytecodeLoader() {}
		virtual void load(Thread* thread, BytecodeDescriptor* bcDescr, byte** out_data, uint32& out_dataLength, uint16& out_instrCount) = 0;
	};

	class BytecodeVerifier
	{
	public:
		virtual ~BytecodeVerifier() {}
		// TODO
	};

	class BytecodeLinker
	{
	public:
		virtual ~BytecodeLinker() {}
		virtual Bytecode* link(Thread* thread, StackRef<Method> method, ClassFileDescriptor* classFile, byte* data, uint32 dataLength, uint16 instrCount) = 0;
	};

	class BytecodeOptimiser
	{
	public:
		virtual ~BytecodeOptimiser() {}
		virtual Bytecode* optimise(Thread* thread, StackRef<Method> method, Bytecode* bc) = 0;
	};

	class BytecodeCompiler
	{
	public:
		virtual ~BytecodeCompiler() {}
		virtual void compile(Thread* thread, StackRef<Method> method, Bytecode* bc) = 0;
	};

	/*class BytecodeExecuter : public BytecodePass
	{
	public:
		// TODO
	};*/

	class BytecodePass
	{
	public:
		virtual ~BytecodePass() {}

		virtual Bytecode* process(Thread* thread, StackRef<Method> method, Bytecode* bc) = 0;
	};

	/*class BytecodeOptimisation : public BytecodePass
	{
	public:
		virtual ~BytecodeOptimisation() {}
		// TODO
	};*/
};