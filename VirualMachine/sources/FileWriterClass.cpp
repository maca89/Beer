#include "stdafx.h"
#include "FileWriterClass.h"
#include "MethodReflection.h"
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
	ret = vm->createBoolean(receiver->open(filename->c_str()));
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
	std::string str;
	arg->toString(vm, str);
	receiver->write(str.c_str());
}

void BEER_CALL BeerFileWriter_writeLn(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver)
{
	receiver->write("\n");
}

void BEER_CALL BeerFileWriter_writeFailed(VirtualMachine* vm, StackFrame* frame, StackRef<FileWriter> receiver, StackRef<Boolean> ret)
{
	ret = vm->createBoolean(receiver->failed());
}


ClassReflection* FileWriterClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<FileWriterClass>(name, 1, 0, 10);
}

void FileWriterClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->extends(0, vm->getClass("Object"));

	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>("FileWriter", "FileWriter::FileWriter()", 1, 0);
	method->setFunction(&BeerFileWriter_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("open", "FileWriter::open(String)", 1, 1);
	method->setFunction(&BeerFileWriter_open);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("close", "FileWriter::close()", 0, 0);
	method->setFunction(&BeerFileWriter_close);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeInteger", "FileWriter::write(Integer)", 0, 1);
	method->setFunction(&BeerFileWriter_writeInteger);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeInteger", "FileWriter::write(Float)", 0, 1);
	method->setFunction(&BeerFileWriter_writeFloat);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeInteger", "FileWriter::write(String)", 0, 1);
	method->setFunction(&BeerFileWriter_writeString);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeInteger", "FileWriter::write(Boolean)", 0, 1);
	method->setFunction(&BeerFileWriter_writeBoolean);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeInteger", "FileWriter::write(Array)", 0, 1);
	method->setFunction(&BeerFileWriter_writeArray);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeInteger", "FileWriter::writeln()", 0, 0);
	method->setFunction(&BeerFileWriter_writeLn);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("writeFailed", "FileWriter::writeFailed()", 1, 0);
	method->setFunction(&BeerFileWriter_writeFailed);
	klass->setMethod(methodi++, method);
}