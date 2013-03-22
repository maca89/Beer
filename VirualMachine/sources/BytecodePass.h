#pragma once
#include "prereq.h"
#include "StackRef.h"


namespace Beer
{
	class Thread;
	class Bytecode;
	class BytecodeDescriptor;
	class ClassFileDescriptor;

	struct TemporaryBytecode
	{
		byte* data;
		uint32 dataLength;

		uint32* dict;
		uint16 instrCount;

		INLINE TemporaryBytecode() : data(NULL), dataLength(0), dict(NULL), instrCount(0) {}
	};

	class BytecodeLoader
	{
	public:
		virtual ~BytecodeLoader() {}
		virtual void load(Thread* thread, BytecodeDescriptor* bcDescr, TemporaryBytecode& out_bc) = 0;
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
		virtual void link(Thread* thread, StackRef<Method> method, ClassFileDescriptor* classFile, const TemporaryBytecode& bc, TemporaryBytecode& out_bc) = 0;
	};

	class BytecodeOptimiser
	{
	public:
		virtual ~BytecodeOptimiser() {}
		virtual void optimise(Thread* thread, StackRef<Method> method, const TemporaryBytecode& bc, TemporaryBytecode& out_bc) = 0;
	};

	class BytecodeCompiler
	{
	public:
		virtual ~BytecodeCompiler() {}
		virtual void compile(Thread* thread, StackRef<Method> method, const TemporaryBytecode& bc) = 0;
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

		virtual void process(Thread* thread, StackRef<Method> method, const TemporaryBytecode& bc, TemporaryBytecode& out_bc) = 0;
	};

	/*class BytecodeOptimisation : public BytecodePass
	{
	public:
		virtual ~BytecodeOptimisation() {}
		// TODO
	};*/
};