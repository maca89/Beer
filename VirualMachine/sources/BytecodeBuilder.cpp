#include "stdafx.h"
#include "BytecodeBuilder.h"
#include "VirtualMachine.h"
#include "Bytecode.h"
#include "BytecodeInputStream.h"
#include "BytecodeOutputStream.h"
#include "ClassFileDescriptor.h"
#include "BytecodeDescriptor.h"
#include "DefaultBytecodeLinker.h"
#include "MultipassBytecodeOptimiser.h"
#include "DefaultBytecodeVerifier.h"

using namespace Beer;


void DefaultBytecodeLoader::load(Thread* thread, BytecodeDescriptor* bcDescr, TemporaryBytecode& out_bc)
{
	out_bc.data = bcDescr->getData();
	out_bc.dataLength = bcDescr->getSize();
	out_bc.instrCount = bcDescr->getInstrCount();
}

BytecodeBuilder::BytecodeBuilder()
	: mLoader(NULL), mLinker(NULL), mCompiler(NULL), mOptimiser(NULL)
{
	mLoader = new DefaultBytecodeLoader(); // TODO
	mLinker = new DefaultBytecodeLinker(); // TODO
	mVerifier = new DefaultBytecodeVerifier(); // TODO
	mCompiler = new DefaultBytecodeCompiler(); // TODO
	mOptimiser = new MultipassBytecodeOptimiser(); // TODO
}

BytecodeBuilder::~BytecodeBuilder()
{
	SMART_DELETE(mLoader); // TODO
	SMART_DELETE(mLinker); // TODO
	SMART_DELETE(mCompiler); // TODO
	SMART_DELETE(mOptimiser); // TODO
}

void BytecodeBuilder::build(Thread* thread, StackRef<Class> klass, StackRef<Method> method, ClassFileDescriptor* klassFile, BytecodeDescriptor* bcDescr)
{
	Bytecode* bc = NULL;
	byte* data = NULL;
	uint32 dataLength = 0;
	uint16 instrCount = 0;

	TemporaryBytecode tmpBc;

	mLoader->load(thread, bcDescr, tmpBc);
	mLinker->link(thread, method, klassFile, tmpBc, tmpBc);
	mVerifier->verify(thread, klass, method, tmpBc);
	mOptimiser->optimise(thread, method, tmpBc, tmpBc);
	
	mCompiler->compile(thread, method, tmpBc); // TODO: return fn pointer
}
