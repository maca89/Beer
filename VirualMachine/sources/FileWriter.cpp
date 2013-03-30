#include "stdafx.h"
#include "FileWriter.h"
#include "Method.h"
#include "VirtualMachine.h"
#include "Frame.h"
#include "String.h"
#include "Boolean.h"
#include "Array.h"

using namespace Beer;


void BEER_CALL FileWriter::init(Thread* thread, StackRef<FileWriter> receiver, StackRef<FileWriter> ret)
{
	receiver->mFile = new std::ofstream(); // memory leak!
	ret = receiver;
}

void BEER_CALL FileWriter::open(Thread* thread, StackRef<FileWriter> receiver, StackRef<String> filename, StackRef<Boolean> ret)
{	
	receiver->mFile->open(filename->c_str());
	ret = Boolean::makeInlineValue(receiver->mFile->is_open());
}

void BEER_CALL FileWriter::close(Thread* thread, StackRef<FileWriter> receiver)
{
	receiver->mFile->close();
}

void BEER_CALL FileWriter::writeInteger(Thread* thread, StackRef<FileWriter> receiver, StackRef<Integer> arg)
{
	(*receiver->mFile) << arg->getData();
}

void BEER_CALL FileWriter::writeFloat(Thread* thread, StackRef<FileWriter> receiver, StackRef<Float> arg)
{
	(*receiver->mFile) << std::setprecision(8) << std::fixed << arg->getData();
}

void BEER_CALL FileWriter::writeString(Thread* thread, StackRef<FileWriter> receiver, StackRef<String> arg)
{
	(*receiver->mFile) << arg->c_str();
}

void BEER_CALL FileWriter::writeBoolean(Thread* thread, StackRef<FileWriter> receiver, StackRef<Boolean> arg)
{
	if(arg->getData()) (*receiver->mFile) << "true";
	else (*receiver->mFile) << "false";
}

void BEER_CALL FileWriter::writeArray(Thread* thread, StackRef<FileWriter> receiver, StackRef<Array> arg)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<String> serialisedArray(frame, frame->stackPush());
	Array::operatorString(thread, arg, serialisedArray);

	FileWriter::writeString(thread, receiver, serialisedArray);

	frame->stackPop(); // pop serialisedArray
}

void BEER_CALL FileWriter::writeLn(Thread* thread, StackRef<FileWriter> receiver)
{
	(*receiver->mFile) << BEER_WIDEN("\n");
}

void BEER_CALL FileWriter::writeFailed(Thread* thread, StackRef<FileWriter> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->mFile->fail());
}


Class* FileWriterClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, 0, 10 + Object::OBJECT_METHODS_COUNT);
}

void FileWriterClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->addParent(thread->getVM()->getObjectClass());

	loader->addMethod(thread, klass, BEER_WIDEN("FileWriter"), BEER_WIDEN("FileWriter::FileWriter()"), &FileWriter::init, 1, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("open"), BEER_WIDEN("FileWriter::open(String)"), &FileWriter::open, 1, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("close"), BEER_WIDEN("FileWriter::close()"), &FileWriter::close, 0, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("write"), BEER_WIDEN("FileWriter::write(Integer)"), &FileWriter::writeInteger, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("write"), BEER_WIDEN("FileWriter::write(Float)"), &FileWriter::writeFloat, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("write"), BEER_WIDEN("FileWriter::write(String)"), &FileWriter::writeString, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("write"), BEER_WIDEN("FileWriter::write(Boolean)"), &FileWriter::writeBoolean, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("write"), BEER_WIDEN("FileWriter::write(Array)"), &FileWriter::writeArray, 0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("writeln"), BEER_WIDEN("FileWriter::writeln()"), &FileWriter::writeLn, 0, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("writeFailed"), BEER_WIDEN("FileWriter::writeFailed()"), &FileWriter::writeFailed, 1, 0);
}