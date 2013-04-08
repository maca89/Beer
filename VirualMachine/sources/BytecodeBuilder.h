#pragma once
#include "prereq.h"
#include "BytecodePass.h"


namespace Beer
{
	class Thread;
	class Method;
	class Bytecode;
	class ClassFileDescriptor;
	class BytecodeDescriptor;

	class BytecodeBuilder
	{
	public:
		typedef std::list<BytecodePass*> PassList;

	protected:
		BytecodeLoader* mLoader;
		BytecodeVerifier* mVerifier;
		BytecodeLinker* mLinker;
		BytecodeCompiler* mCompiler;
		BytecodeOptimiser* mOptimiser;

	public:
		BytecodeBuilder();
		~BytecodeBuilder();

		INLINE void setLoader(BytecodeLoader* pass) { mLoader = pass; }
		INLINE BytecodeLoader* getLoader() const { return mLoader; }

		INLINE void setVerifier(BytecodeVerifier* pass) { mVerifier = pass; }
		INLINE BytecodeVerifier* getVerifier() const { return mVerifier; }

		INLINE void setLinker(BytecodeLinker* pass) { mLinker = pass; }
		INLINE BytecodeLinker* getLinker() const { return mLinker; }

		INLINE void setCompiler(BytecodeCompiler* pass) { mCompiler = pass; }
		INLINE BytecodeCompiler* getCompiler() const { return mCompiler; }

		INLINE void setOptimiser(BytecodeOptimiser* pass) { mOptimiser = pass; }
		INLINE BytecodeOptimiser* getOptimiser() const { return mOptimiser; }

		void build(Thread* thread, StackRef<Class> klass, StackRef<Method> method, ClassFileDescriptor* klassFile, BytecodeDescriptor* bytecodeDescr);
		//Bytecode* build(Thread* thread, Bytecode* bc);
	};

	class DefaultBytecodeLoader : public BytecodeLoader
	{
	public:
		virtual void load(Thread* thread, BytecodeDescriptor* bcDescr, TemporaryBytecode& out_bc);
	};
};
