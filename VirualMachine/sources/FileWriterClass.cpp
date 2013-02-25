#include "stdafx.h"
#include "FileWriterClass.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "StringClass.h"
#include "BooleanClass.h"
#include "ArrayClass.h"

using namespace Beer;


void BEER_CALL BeerFileWriter_init(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<FileWriter> ret)
{
	ret = receiver;
}

void BEER_CALL BeerFileWriter_open(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<String> filename, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->open(filename->c_str()));
}

void BEER_CALL BeerFileWriter_close(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver)
{
	receiver->close();
}

void BEER_CALL BeerFileWriter_writeInteger(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<Integer> arg)
{
	receiver->write(arg->getData());
}

void BEER_CALL BeerFileWriter_writeFloat(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<Float> arg)
{
	receiver->write(arg->getData());
}

void BEER_CALL BeerFileWriter_writeString(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<String> arg)
{
	receiver->write(arg->c_str());
}

void BEER_CALL BeerFileWriter_writeBoolean(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<Boolean> arg)
{
	receiver->write(arg->getData());
}

void BEER_CALL BeerFileWriter_writeArray(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<Array> arg)
{
	string str;
	arg->toString(vm, str);
	receiver->write(str.c_str());
}

void BEER_CALL BeerFileWriter_writeLn(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver)
{
	receiver->write(BEER_WIDEN("\n"));
}

void BEER_CALL BeerFileWriter_writeFailed(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->failed());
}


Class* FileWriterClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<FileWriterClass>(name, 1, 0, 10);
}

void FileWriterClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());

	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("FileWriter")));
	method->setFunction(&BeerFileWriter_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::FileWriter()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("open")));
	method->setFunction(&BeerFileWriter_open);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::open(String)")), method));

	method = loader->createMethod(0, 0);
	method->setName(vm->createString(BEER_WIDEN("close")));
	method->setFunction(&BeerFileWriter_close);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::close()")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("write")));
	method->setFunction(&BeerFileWriter_writeInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::write(Integer)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("write")));
	method->setFunction(&BeerFileWriter_writeFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::write(Float)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("write")));
	method->setFunction(&BeerFileWriter_writeString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::write(String)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("write")));
	method->setFunction(&BeerFileWriter_writeBoolean);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::write(Boolean)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("write")));
	method->setFunction(&BeerFileWriter_writeArray);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::write(Array)")), method));

	method = loader->createMethod(0, 0);
	method->setName(vm->createString(BEER_WIDEN("writeln")));
	method->setFunction(&BeerFileWriter_writeLn);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::writeln()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("writeFailed")));
	method->setFunction(&BeerFileWriter_writeFailed);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileWriter::writeFailed()")), method));
}