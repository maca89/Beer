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


Class* FileReaderClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<FileReaderClass>(name, 1, 0, 8);
}

void FileReaderClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());

	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("FileReader")));
	method->setFunction(&BeerFileReader_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::FileReader()")), method));

	method = loader->createMethod<MethodReflection>(1, 1);
	method->setName(vm->createString(BEER_WIDEN("open")));
	method->setFunction(&BeerFileReader_open);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::open(String)")), method));

	method = loader->createMethod<MethodReflection>(0, 0);
	method->setName(vm->createString(BEER_WIDEN("close")));
	method->setFunction(&BeerFileReader_close);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::close()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readInteger")));
	method->setFunction(&BeerFileReader_readInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::readInteger()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readFloat")));
	method->setFunction(&BeerFileReader_readFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::readFloat()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readString")));
	method->setFunction(&BeerFileReader_readString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::readString()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readFailed")));
	method->setFunction(&BeerFileReader_readFailed);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::readFailed()")), method));

	method = loader->createMethod<MethodReflection>(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readEnded")));
	method->setFunction(&BeerFileReader_readEnded);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("FileReader::readEnded()")), method));
}