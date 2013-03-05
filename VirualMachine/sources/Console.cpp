#include "stdafx.h"
#include "Console.h"
#include "Method.h"
#include "Integer.h"
#include "String.h"
#include "Float.h"
#include "Boolean.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "Array.h"

using namespace Beer;

CachedOutput Console::gOut = CachedOutput();
Console::ConsoleArguments Console::gArguments = Console::ConsoleArguments();

void BEER_CALL Console::init(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Console> ret)
{
	receiver->setReadFailed(false);
	ret = receiver;
}

void BEER_CALL Console::println(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver)
{
	Console::getOutput() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printInteger(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Integer> arg)
{
	Console::getOutput() << arg->getData();
}

void BEER_CALL Console::printlnInteger(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Integer> arg)
{
	Console::getOutput() << arg->getData() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printFloat(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Float> arg)
{
	Console::getOutput() << std::setprecision(8) << std::fixed << arg->getData();
}

void BEER_CALL Console::printlnFloat(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Float> arg)
{
	Console::getOutput() << std::setprecision(8) << std::fixed << arg->getData() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printString(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<String> arg)
{
	Console::getOutput() << arg.get();
}

void BEER_CALL Console::printlnString(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<String> arg)
{
	Console::getOutput() << arg.get() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printBoolean(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Boolean> arg)
{
	Console::getOutput() << arg->getData();
}

void BEER_CALL Console::printlnBoolean(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Boolean> arg)
{
	Console::getOutput() << arg->getData() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::printArray(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Array> arg)
{
	// TODO
	string str;
	arg->toString(thread->getVM(), str);
	Console::getOutput() << str;
}

void BEER_CALL Console::printlnArray(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Array> arg)
{
	// TODO
	string str;
	arg->toString(thread->getVM(), str);
	Console::getOutput() << str << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL Console::readInteger(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Integer> ret)
{
	Console::getOutput().flush(cout); // important

	Integer::IntegerData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	thread->createInteger(ret, n);
}

void BEER_CALL Console::readFloat(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Float> ret)
{
	Console::getOutput().flush(cout); // important

	Float::FloatData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	thread->createFloat(ret, n);
}

void BEER_CALL Console::readBoolean(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Boolean> ret)
{
	Console::getOutput().flush(cout); // important

	Boolean::BooleanData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = Boolean::makeInlineValue(n);
}

void BEER_CALL Console::readLine(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<String> ret)
{
	Console::getOutput().flush(cout); // important

	string str;
	std::getline(cin, str);
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = thread->getVM()->createString(str);
}

void BEER_CALL Console::readFailed(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Boolean> ret)
{
	ret = Boolean::makeInlineValue(receiver->getReadFailed());
}

void BEER_CALL Console::getArg(Thread* thread/*, StackFrame* frame*/, StackRef<Console> receiver, StackRef<Integer> index, StackRef<String> ret)
{
	Integer::IntegerData i = index->getData();
	if(Console::gHasArg(i)) ret = thread->getVM()->createString(Console::gGetArg(i));
	else ret = thread->getVM()->createString(BEER_WIDEN(""));
}


Class* ConsoleClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(name, 1, 0, 18);
}

void ConsoleClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());

	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Console")));
	method->setFunction(&Console::init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::Console()")), method));

	method = loader->createMethod(0, 0);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&Console::println);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println()")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("print")));
	method->setFunction(&Console::printInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::print(Integer)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&Console::printlnInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println(Integer)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("print")));
	method->setFunction(&Console::printFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::print(Float)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&Console::printlnFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println(Float)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("print")));
	method->setFunction(&Console::printString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::print(String)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&Console::printlnString);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println(String)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("print")));
	method->setFunction(&Console::printBoolean);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::print(Boolean)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&Console::printlnBoolean);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println(Boolean)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("print")));
	method->setFunction(&Console::printArray);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::print(Array)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&Console::printlnArray);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println(Array)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readInteger")));
	method->setFunction(&Console::readInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readInteger()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readFloat")));
	method->setFunction(&Console::readFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readFloat()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readBoolean")));
	method->setFunction(&Console::readBoolean);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readBoolean()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readLine")));
	method->setFunction(&Console::readLine);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readLine()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readFailed")));
	method->setFunction(&Console::readFailed);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readFailed()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("getArg")));
	method->setFunction(&Console::getArg);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::getArg(Integer)")), method));
}