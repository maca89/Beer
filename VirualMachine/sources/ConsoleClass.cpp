#include "stdafx.h"
#include "ConsoleClass.h"
#include "Method.h"
#include "IntegerClass.h"
#include "StringClass.h"
#include "FloatClass.h"
#include "BooleanClass.h"
#include "VirtualMachine.h"
#include "StackFrame.h"
#include "ArrayClass.h"

using namespace Beer;

CachedOutput Console::gOut = CachedOutput();
Console::ConsoleArguments Console::gArguments = Console::ConsoleArguments();

// print

#define BuildPrintMethodFn(Name, Param, Operation)																		\
struct BeerConsole_##Name##Param																						\
{																														\
	static void BEER_CALL fn(																							\
		VirtualMachine* vm, 																							\
		StackFrame* frame, 																								\
		StackRef<Console> receiver, 																					\
		StackRef<Param> value) 																							\
	{																													\
		Console::getOutput() << Operation;																				\
	}																													\
};																														\

#define BuildPrintMethod(Name, Param, Operation)																		\
	BuildPrintMethodFn(Name, Param, Operation);																			\
	method = loader->createMethod(																				\
			1, 																											\
			1																											\
		);																												\
	method->setName(vm->createString(BEER_WIDEN("Console")));															\
	/*Name##Method#Method->getReturn(0)->setType(integerClass);*/														\
	/*Name##Method#Method->getParam(0)->setType(integerClass);*/														\
	method->setFunction(&(BeerConsole_##Name##Param::fn));																\
	klass->setMethod(methodi++, vm->createPair(vm->createString((string(BEER_WIDEN("Console::")) + BEER_WIDEN(#Name) + BEER_WIDEN("(") + BEER_WIDEN(#Param) + BEER_WIDEN(")")).c_str()), method));\


void BEER_CALL BeerConsole_init(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Console> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerConsole_println(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver)
{
	Console::getOutput() << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL BeerConsole_printArray(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Array> arg)
{
	// TODO
	string str;
	arg->toString(vm, str);
	Console::getOutput() << str;
}

void BEER_CALL BeerConsole_printlnArray(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Array> arg)
{
	// TODO
	string str;
	arg->toString(vm, str);
	Console::getOutput() << str << "\n"; // FOR PERFORMANCE REASONS DO NOT USE std::endl
}

void BEER_CALL BeerConsole_readInteger(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Integer> ret)
{
	Console::getOutput().flush(cout); // important

	Integer::IntegerData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createInteger(n);
}

void BEER_CALL BeerConsole_readFloat(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Float> ret)
{
	Console::getOutput().flush(cout); // important

	Float::FloatData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createFloat(n);
}

void BEER_CALL BeerConsole_readBoolean(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Boolean> ret)
{
	Console::getOutput().flush(cout); // important

	Boolean::BooleanData n = 0;
	cin >> n;
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createFloat(n);
}

void BEER_CALL BeerConsole_readLine(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<String> ret)
{
	Console::getOutput().flush(cout); // important

	string str;
	std::getline(cin, str);
	if(cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createString(str);
}

void BEER_CALL BeerConsole_readFailed(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<String> ret)
{
	ret = Boolean::makeInlineValue(receiver->getReadFailed());
}

void BEER_CALL BeerConsole_getArg(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Integer> index, StackRef<String> ret)
{
	Integer::IntegerData i = index->getData();
	if(Console::hasArg(i)) ret = vm->createString(Console::getArg(i));
	else ret = vm->createString(BEER_WIDEN(""));
}


Class* ConsoleClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	return loader->createClass<ConsoleClass>(name, 1, 0, 18);
}

void ConsoleClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass)
{
	klass->extends(0, vm->getObjectClass());

	Method* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("Console")));
	method->setFunction(&BeerConsole_init);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::Console()")), method));

	method = loader->createMethod(0, 0);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&BeerConsole_println);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println()")), method));

	BuildPrintMethod(print, Integer, value->getData());
	BuildPrintMethod(println, Integer, value->getData() << '\n'); // FOR PERFORMANCE REASONS DO NOT USE std::endl

	BuildPrintMethod(print, Float, std::setprecision(8) << std::fixed << value->getData());
	BuildPrintMethod(println, Float, std::setprecision(8) << std::fixed << value->getData() << '\n'); // FOR PERFORMANCE REASONS DO NOT USE std::endl

	BuildPrintMethod(print, String, value.get());
	BuildPrintMethod(println, String, value.get() << '\n'); // FOR PERFORMANCE REASONS DO NOT USE std::endl

	BuildPrintMethod(print, Boolean, value->getData());
	BuildPrintMethod(println, Boolean, value->getData() << '\n'); // FOR PERFORMANCE REASONS DO NOT USE std::endl

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("print")));
	method->setFunction(&BeerConsole_printArray);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::print(Array)")), method));

	method = loader->createMethod(0, 1);
	method->setName(vm->createString(BEER_WIDEN("println")));
	method->setFunction(&BeerConsole_printlnArray);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::println(Array)")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readInteger")));
	method->setFunction(&BeerConsole_readInteger);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readInteger()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readFloat")));
	method->setFunction(&BeerConsole_readFloat);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readFloat()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readBoolean")));
	method->setFunction(&BeerConsole_readBoolean);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readBoolean()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readLine")));
	method->setFunction(&BeerConsole_readLine);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readLine()")), method));

	method = loader->createMethod(1, 0);
	method->setName(vm->createString(BEER_WIDEN("readFailed")));
	method->setFunction(&BeerConsole_readFailed);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::readFailed()")), method));

	method = loader->createMethod(1, 1);
	method->setName(vm->createString(BEER_WIDEN("getArg")));
	method->setFunction(&BeerConsole_getArg);
	klass->setMethod(methodi++, vm->createPair(vm->createString(BEER_WIDEN("Console::getArg(Integer)")), method));
}