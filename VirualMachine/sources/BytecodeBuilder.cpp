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

using namespace Beer;


void DefaultBytecodeLoader::load(Thread* thread, BytecodeDescriptor* bcDescr, byte** out_data, uint32& out_dataLength, uint16& out_instrCount)
{
	*out_data = bcDescr->getData();
	out_dataLength = bcDescr->getSize();
	out_instrCount = bcDescr->getInstrCount();
}

BytecodeBuilder::BytecodeBuilder()
	: mLoader(NULL), mLinker(NULL), mCompiler(NULL), mOptimiser(NULL)
{
	mLoader = new DefaultBytecodeLoader(); // TODO
	mLinker = new DefaultBytecodeLinker(); // TODO
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

void BytecodeBuilder::build(Thread* thread, StackRef<Method> method, ClassFileDescriptor* klassFile, BytecodeDescriptor* bcDescr)
{
	Bytecode* bc = NULL;
	byte* data = NULL;
	uint32 dataLength = 0;
	uint16 instrCount = 0;

	mLoader->load(thread, bcDescr, &data, dataLength, instrCount);
	//mVerifier->verify(thread, klassFile, data, dataLength, instrCount);

	bc = mLinker->link(thread, method, klassFile, data, dataLength, instrCount); // new Bytecode(data, dataLength, instrCount);
	bc = mOptimiser->optimise(thread, method, bc);
	
	mCompiler->compile(thread, method, bc); // TODO: return fn pointer
}
