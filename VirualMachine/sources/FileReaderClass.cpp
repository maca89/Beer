#include "stdafx.h"
#include "FileReaderClass.h"
#include "MethodReflection.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "StringClass.h"
#include "BooleanClass.h"
#include "FloatClass.h"
#include "StringClass.h"

using namespace Beer;


void BEER_CALL BeerFileReader_init(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<FileReader> ret)
{
	ret = receiver;
}

void BEER_CALL BeerFileReader_open(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<String> filename, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->open(filename->c_str()));
}

void BEER_CALL BeerFileReader_close(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver)
{
	receiver->close();
}

void BEER_CALL BeerFileReader_readInteger(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<Integer> ret)
{
	Integer::IntegerData data;
	receiver->read(data);
	ret = vm->createInteger(data);
}

void BEER_CALL BeerFileReader_readFloat(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<Float> ret)
{
	Float::FloatData data;
	receiver->read(data);
	ret = vm->createFloat(data);
}

void BEER_CALL BeerFileReader_readString(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<String> ret)
{
	string data;
	receiver->read(data);
	ret = vm->createString(data);
}

void BEER_CALL BeerFileReader_readFailed(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->failed());
}

void BEER_CALL BeerFileReader_readEnded(VirtualMachine* vm, StackFrame* frame, StackRef<FileReader> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->atEnd());
}


ClassReflection* FileReaderClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, string name)
{
	return loader->createClass<FileReaderClass>(name, 1, 0, 8);
}

void FileReaderClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->extends(0, vm->getObjectClass());

	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("FileReader"), BEER_WIDEN("FileReader::FileReader()"), 1, 0);
	method->setFunction(&BeerFileReader_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("open"), BEER_WIDEN("FileReader::open(String)"), 1, 1);
	method->setFunction(&BeerFileReader_open);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("close"), BEER_WIDEN("FileReader::close()"), 0, 0);
	method->setFunction(&BeerFileReader_close);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("readInteger"), BEER_WIDEN("FileReader::readInteger()"), 1, 0);
	method->setFunction(&BeerFileReader_readInteger);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("readFloat"), BEER_WIDEN("FileReader::readFloat()"), 1, 0);
	method->setFunction(&BeerFileReader_readFloat);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("readString"), BEER_WIDEN("FileReader::readString()"), 1, 0);
	method->setFunction(&BeerFileReader_readString);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("readFailed"), BEER_WIDEN("FileReader::readFailed()"), 1, 0);
	method->setFunction(&BeerFileReader_readFailed);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>(BEER_WIDEN("readEnded"), BEER_WIDEN("FileReader::readEnded()"), 1, 0);
	method->setFunction(&BeerFileReader_readEnded);
	klass->setMethod(methodi++, method);
}