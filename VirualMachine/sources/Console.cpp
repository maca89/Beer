#include "stdafx.h"
#include "Console.h"
#include "Method.h"
#include "Integer.h"
#include "String.h"
#include "Float.h"
#include "Boolean.h"
#include "VirtualMachine.h"
#include "Frame.h"
#include "Array.h"

using namespace Beer;

Console::Output Console::gOut = Console::Output();
Console::ConsoleArguments Console::gArguments = Console::ConsoleArguments();

void BEER_CALL Console::init(Thread* thread, StackRef<Console> receiver, StackRef<Console> ret)
{
	receiver->setReadFailed(false);
	ret = receiver;
}

void BEER_CALL Console::println(Thread* thread, StackRef<Console> receiver)
{
	Console::getOutput() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printInteger(Thread* thread, StackRef<Console> receiver, StackRef<Integer> arg)
{
	Console::getOutput() << arg->getData();
}

void BEER_CALL Console::printlnInteger(Thread* thread, StackRef<Console> receiver, StackRef<Integer> arg)
{
	Console::getOutput() << arg->getData() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printFloat(Thread* thread, StackRef<Console> receiver, StackRef<Float> arg)
{
	Console::getOutput() << std::setprecision(8) << std::fixed << arg->getData();
}

void BEER_CALL Console::printlnFloat(Thread* thread, StackRef<Console> receiver, StackRef<Float> arg)
{
	Console::getOutput() << std::setprecision(8) << std::fixed << arg->getData() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printString(Thread* thread, StackRef<Console> receiver, StackRef<String> arg)
{
	Console::getOutput() << arg.get();
}

void BEER_CALL Console::printlnString(Thread* thread, StackRef<Console> receiver, StackRef<String> arg)
{
	Console::getOutput() << arg.get() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printBoolean(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> arg)
{
	Console::getOutput() << arg->getData();
}

void BEER_CALL Console::printlnBoolean(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> arg)
{
	Console::getOutput() << arg->getData() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printArray(Thread* thread, StackRef<Console> receiver, StackRef<Array> arg)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<String> serialisedArray(frame, frame->stackPush());
	Array::operatorString(thread, arg, serialisedArray);

	Console::printString(thread, receiver, serialisedArray);

	frame->stackPop(); // pop serialisedArray
}

void BEER_CALL Console::printlnArray(Thread* thread, StackRef<Console> receiver, StackRef<Array> arg)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<String> serialisedArray(frame, frame->stackPush());
	Array::operatorString(thread, arg, serialisedArray);

	Console::printString(thread, receiver, serialisedArray);
	Console::getOutput() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
	
	frame->stackPop(); // pop serialisedArray
}

void BEER_CALL Console::readInteger(Thread* thread, StackRef<Console> receiver, StackRef<Integer> ret)
{
	//Console::getOutput().flush(cout); // important

	Integer::IntegerData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	thread->createInteger(ret, n);
}

void BEER_CALL Console::readFloat(Thread* thread, StackRef<Console> receiver, StackRef<Float> ret)
{
	//Console::getOutput().flush(cout); // important

	Float::FloatData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	thread->createFloat(ret, n);
}

void BEER_CALL Console::readBoolean(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> ret)
{
	//Console::getOutput().flush(cout); // important

	Boolean::BooleanData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = Boolean::makeInlineValue(n);
}

void BEER_CALL Console::readLine(Thread* thread, StackRef<Console> receiver, StackRef<String> ret)
{
	//Console::getOutput().flush(cout); // important

	string str;
	std::getline(cin, str);
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	thread->createString(ret, str);
}

void BEER_CALL Console::readFailed(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getReadFailed());
}

void BEER_CALL Console::getArg(Thread* thread, StackRef<Console> receiver, StackRef<Integer> index, StackRef<String> ret)
{
	Integer::IntegerData i = index->getData();
	if(Console::gHasArg(i))
	{
		thread->createString(ret, Console::gGetArg(i));
	}
	else
	{
		thread->createString(ret, BEER_WIDEN(""));
	}
}

Class* ConsoleClassInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, 0, 18 + Object::OBJECT_METHODS_COUNT);
}

void ConsoleClassInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->addParent(thread->getVM()->getObjectClass());
	
	loader->addMethod(thread, klass, BEER_WIDEN("Console"), BEER_WIDEN("Console::Console()"), &Console::init, 1, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("println"), BEER_WIDEN("Console::println()"), &Console::println, 0, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("print"),	BEER_WIDEN("Console::print(Integer)"),		&Console::printInteger,		0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("println"), BEER_WIDEN("Console::println(Integer)"),	&Console::printlnInteger,	0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("print"),	BEER_WIDEN("Console::print(Float)"),		&Console::printFloat,		0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("println"), BEER_WIDEN("Console::println(Float)"),		&Console::printlnFloat,		0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("print"),	BEER_WIDEN("Console::print(String)"),		&Console::printString,		0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("println"), BEER_WIDEN("Console::println(String)"),		&Console::printlnString,	0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("print"),	BEER_WIDEN("Console::print(Boolean)"),		&Console::printBoolean,		0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("println"), BEER_WIDEN("Console::println(Boolean)"),	&Console::printlnBoolean,	0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("print"),	BEER_WIDEN("Console::print(Array)"),		&Console::printArray,		0, 1);
	loader->addMethod(thread, klass, BEER_WIDEN("println"), BEER_WIDEN("Console::println(Array)"),		&Console::printlnArray,		0, 1);

	loader->addMethod(thread, klass, BEER_WIDEN("readInteger"), BEER_WIDEN("Console::readInteger()"),	&Console::readInteger,	1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("readFloat"),	BEER_WIDEN("Console::readFloat()"),		&Console::readFloat,	1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("readBoolean"), BEER_WIDEN("Console::readBoolean()"),	&Console::readBoolean,	1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("readLine"),	BEER_WIDEN("Console::readLine()"),		&Console::readLine,		1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("readFailed"),	BEER_WIDEN("Console::readFailed()"),	&Console::readFailed,	1, 0);

	loader->addMethod(thread, klass, BEER_WIDEN("getArg"), BEER_WIDEN("Console::getArg(Integer)"), &Console::getArg, 1, 1);
}