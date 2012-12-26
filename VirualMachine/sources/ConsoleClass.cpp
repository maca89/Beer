#include "stdafx.h"
#include "ConsoleClass.h"
#include "MethodReflection.h"
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
	method = loader->createMethod<MethodReflection>(																	\
			#Name, 																										\
			std::string(klass->getName()) + "::" + #Name + "(" + #Param + ")", 											\
			1, 																											\
			1																											\
		);																												\
	/*Name##Method#Method->getReturn(0)->setType(integerClass);*/														\
	/*Name##Method#Method->getParam(0)->setType(integerClass);*/														\
	method->setFunction(&(BeerConsole_##Name##Param::fn));																\
	klass->setMethod(methodi++, method);																				\


void BEER_CALL BeerConsole_init(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Console> ret1)
{
	ret1 = receiver;
}

void BEER_CALL BeerConsole_println(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver)
{
	Console::getOutput() << std::endl;
}

void BEER_CALL BeerConsole_printArray(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Array> arg)
{
	// TODO
	std::string str;
	arg->toString(str);
	Console::getOutput() << str;
}

void BEER_CALL BeerConsole_printlnArray(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Array> arg)
{
	// TODO
	std::string str;
	arg->toString(str);
	Console::getOutput() << str << std::endl;
}

void BEER_CALL BeerConsole_readInteger(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Integer> ret)
{
	Console::getOutput().flush(std::cout); // important

	Integer::IntegerData n = 0;
	std::cin >> n;
	if(std::cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createInteger(n);
}

void BEER_CALL BeerConsole_readFloat(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Float> ret)
{
	Console::getOutput().flush(std::cout); // important

	Float::FloatData n = 0;
	std::cin >> n;
	if(std::cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createFloat(n);
}

void BEER_CALL BeerConsole_readBoolean(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Boolean> ret)
{
	Console::getOutput().flush(std::cout); // important

	Boolean::BooleanData n = 0;
	std::cin >> n;
	if(std::cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createFloat(n);
}

void BEER_CALL BeerConsole_readLine(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<String> ret)
{
	Console::getOutput().flush(std::cout); // important

	std::string str;
	std::getline(std::cin, str);
	if(std::cin.fail()) { receiver->setReadFailed(true); return; }
	receiver->setReadFailed(false);
	ret = vm->createString(str);
}

void BEER_CALL BeerConsole_readFailed(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<String> ret)
{
	ret = vm->createBoolean(receiver->getReadFailed());
}

void BEER_CALL BeerConsole_getArg(VirtualMachine* vm, StackFrame* frame, StackRef<Console> receiver, StackRef<Integer> index, StackRef<String> ret)
{
	Integer::IntegerData i = index->getData();
	if(Console::hasArg(i)) ret = vm->createString(Console::getArg(i));
	else ret = vm->createString("");
}


ClassReflection* ConsoleClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<ConsoleClass>(name, 1, 0, 18);
}

void ConsoleClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->extends(0, vm->getClass("Object"));

	MethodReflection* method = NULL;
	uint16 methodi = 0;

	method = loader->createMethod<MethodReflection>("Console", std::string(klass->getName()) + "::Console()", 1, 0);
	method->setFunction(&BeerConsole_init);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("println", std::string(klass->getName()) + "::println()", 0, 0);
	method->setFunction(&BeerConsole_println);
	klass->setMethod(methodi++, method);

	BuildPrintMethod(print, Integer, value->getData());
	BuildPrintMethod(println, Integer, value->getData() << std::endl);

	BuildPrintMethod(print, Float, std::setprecision(8) << std::fixed << value->getData());
	BuildPrintMethod(println, Float, std::setprecision(8) << std::fixed << value->getData() << std::endl);

	BuildPrintMethod(print, String, value->c_str());
	BuildPrintMethod(println, String, value->c_str() << std::endl);

	BuildPrintMethod(print, Boolean, value->getData());
	BuildPrintMethod(println, Boolean, value->getData() << std::endl);

	method = loader->createMethod<MethodReflection>("print", std::string(klass->getName()) + "::print(Array)", 0, 1);
	method->setFunction(&BeerConsole_printArray);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("println", std::string(klass->getName()) + "::println(Array)", 0, 1);
	method->setFunction(&BeerConsole_printlnArray);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("readInteger", std::string(klass->getName()) + "::readInteger()", 1, 0);
	method->setFunction(&BeerConsole_readInteger);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("readFloat", std::string(klass->getName()) + "::readFloat()", 1, 0);
	method->setFunction(&BeerConsole_readFloat);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("readBoolean", std::string(klass->getName()) + "::readBoolean()", 1, 0);
	method->setFunction(&BeerConsole_readBoolean);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("readLine", std::string(klass->getName()) + "::readLine()", 1, 0);
	method->setFunction(&BeerConsole_readLine);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("readFailed", std::string(klass->getName()) + "::readFailed()", 1, 0);
	method->setFunction(&BeerConsole_readFailed);
	klass->setMethod(methodi++, method);

	method = loader->createMethod<MethodReflection>("getArg", std::string(klass->getName()) + "::getArg(Integer)", 1, 1);
	method->setFunction(&BeerConsole_getArg);
	klass->setMethod(methodi++, method);
}