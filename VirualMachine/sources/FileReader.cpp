#include "stdafx.h"
#include "FileReader.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Frame.h"
#include "String.h"
#include "Boolean.h"
#include "Float.h"
#include "String.h"

using namespace Beer;


void BEER_CALL FileReader::init(Thread* thread, StackRef<FileReader> receiver, StackRef<FileReader> ret)
{
	receiver->mFile = new ifstream(); // memory leak!
	ret = receiver;
}

void BEER_CALL FileReader::open(Thread* thread, StackRef<FileReader> receiver, StackRef<String> filename, StackRef<Boolean> ret)
{
	receiver->mFile->open(filename->c_str());
	ret = Boolean::makeInlineValue(receiver->mFile->is_open());
}

void BEER_CALL FileReader::close(Thread* thread, StackRef<FileReader> receiver)
{
	receiver->mFile->close();
}

void BEER_CALL FileReader::readInteger(Thread* thread, StackRef<FileReader> receiver, StackRef<Integer> ret)
{
	Integer::IntegerData data;
	(*receiver->mFile) >> data;
	thread->createInteger(ret, data);
}

void BEER_CALL FileReader::readFloat(Thread* thread, StackRef<FileReader> receiver, StackRef<Float> ret)
{
	Float::FloatData data;
	(*receiver->mFile) >> data;
	thread->createFloat(ret, data);
}

void BEER_CALL FileReader::readString(Thread* thread, StackRef<FileReader> receiver, StackRef<String> ret)
{
	string data;
	(*receiver->mFile) >> data;
	ret = thread->getVM()->createString(data);
}

void BEER_CALL FileReader::readFailed(Thread* thread, StackRef<FileReader> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->mFile->fail());
}

void BEER_CALL FileReader::readEnded(Thread* thread, StackRef<FileReader> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->mFile->eof());
}


Class* FileReaderClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 8);
}

void FileReaderClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	loader->extendClass(klass, vm->getObjectClass());

	Method* method = NULL;
	uint16 methodi = 0;

	loader->addMethod(klass, BEER_WIDEN("FileReader"), BEER_WIDEN("FileReader::FileReader()"), &FileReader::init, 1, 0);

	loader->addMethod(klass, BEER_WIDEN("open"), BEER_WIDEN("FileReader::open(String)"), &FileReader::open, 1, 1);
	loader->addMethod(klass, BEER_WIDEN("close"), BEER_WIDEN("FileReader::close()"), &FileReader::close, 0, 0);

	loader->addMethod(klass, BEER_WIDEN("readInteger"), BEER_WIDEN("FileReader::readInteger()"), &FileReader::readInteger, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("readFloat"), BEER_WIDEN("FileReader::readFloat()"), &FileReader::readFloat, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("readString"), BEER_WIDEN("FileReader::readString()"), &FileReader::readString, 1, 0);

	loader->addMethod(klass, BEER_WIDEN("readFailed"), BEER_WIDEN("FileReader::readFailed()"), &FileReader::readFailed, 1, 0);
	loader->addMethod(klass, BEER_WIDEN("readEnded"), BEER_WIDEN("FileReader::readEnded()"), &FileReader::readEnded, 1, 0);
}