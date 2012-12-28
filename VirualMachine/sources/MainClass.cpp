#include "stdafx.h"
#include "MainClass.h"
#include "MethodReflection.h"
#include "Bytecode.h"
#include "IntegerClass.h"
#include "StringClass.h"
#include "VirtualMachine.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "BytecodeBuilder.h"

using namespace Beer;


void BEER_CALL BeerMain_foo(VirtualMachine* vm, StackFrame* frame, StackRef<Main> receiver, StackRef<Integer> var1, StackRef<Integer> var2, StackRef<Integer> ret1)
{
	ret1 = vm->createInteger(42);
}

ClassReflection* MainClassInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<MainClass>(name, 1, 0, 2);
}

void MainClassInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass)
{
	klass->extends(0, vm->getObjectClass());

	ClassFileDescriptor* classFile = reinterpret_cast<ClassFileDescriptor*>(new byte[2048]);
	memset(classFile, 0, 2048 * sizeof(byte));

	classFile->getDescriptorsLength() = 4;

	// descr#0
	classFile->getDescriptorAddr(0) = 100;
	classFile->getDescriptor<StringDescriptor>(0)->size() = 7;
	memcpy(classFile->getDescriptor<StringDescriptor>(0)->c_str(), "Integer", classFile->getDescriptor<StringDescriptor>(0)->size());

	// descr#1
	classFile->getDescriptorAddr(1) = 200;
	classFile->getDescriptor<StringDescriptor>(1)->size() = 7;
	memcpy(classFile->getDescriptor<StringDescriptor>(1)->c_str(), "Console", classFile->getDescriptor<StringDescriptor>(1)->size());

	// descr#2
	classFile->getDescriptorAddr(2) = 300;
	classFile->getDescriptor<StringDescriptor>(2)->size() = 27;
	memcpy(classFile->getDescriptor<StringDescriptor>(2)->c_str(), "Main::foo(Integer,Integer)", classFile->getDescriptor<StringDescriptor>(2)->size());

	// descr#3
	classFile->getDescriptorAddr(3) = 400;
	classFile->getDescriptor<StringDescriptor>(3)->size() = 23;
	memcpy(classFile->getDescriptor<StringDescriptor>(3)->c_str(), "Console::print(Integer)", classFile->getDescriptor<StringDescriptor>(3)->size());

	// external classes
	classFile->getExternalClassesLength() = 2;
	classFile->getClassId(0) = 0; // Integer
	classFile->getClassId(1) = 1; // Console

	uint16 instrs = 0;
	uint16 i = 0;
	uint16 bcLength = 100;
	byte* bc = new byte[bcLength];
	memset(bc, 0, bcLength*sizeof(byte));
	//////////////////////////////////////////////////////////////////

	bc[i++]= Bytecode::INSTR_MOVE_TOP;
	bc[i++] = 1;
	bc[i++] = 0;
	instrs++;

	bc[i++] = Bytecode::INSTR_PUSH_INT8;
	bc[i++] = 1; // new Integer(1)
	instrs++;

	bc[i++] = Bytecode::INSTR_PUSH_INT8;
	bc[i++] = 2; // new Integer(2)
	instrs++;

	bc[i++] = Bytecode::INSTR_TOP;
	bc[i++] = 0; // this
	bc[i++] = 0;
	instrs++;

	bc[i++] = Bytecode::INSTR_INVOKE;
	bc[i++] = 2; // Main::foo(Integer,Integer)
	bc[i++] = 0;
	bc[i++] = 0;
	bc[i++] = 0;
	instrs++;

	bc[i++] = Bytecode::INSTR_NEW;
	bc[i++] = 1; // Console
	bc[i++] = 0;
	bc[i++] = 0;
	bc[i++] = 0;
	instrs++;

	bc[i++] = Bytecode::INSTR_INVOKE;
	bc[i++] = 3; // Console::print(Integer)
	bc[i++] = 0;
	bc[i++] = 0;
	bc[i++] = 0;
	instrs++;

	bc[i++] = Bytecode::INSTR_RETURN;
	bc[i++] = 0;
	instrs++;

	//////////////////////////////////////////////////////////////////
	MethodReflection* runMethod = loader->createMethod<MethodReflection>("run", std::string(klass->getName()) + "::run()", 0, 0);
	Bytecode* bytecode = new Bytecode(bc, i, instrs);
	
	bytecode->build(vm, classFile);
	runMethod->setBytecode(bytecode);	
	klass->setMethod(0, runMethod);

	MethodReflection* fooMethod = loader->createMethod<MethodReflection>("foo", std::string(klass->getName()) + "::foo(Integer,Integer)", 1, 2);
	fooMethod->setFunction(&BeerMain_foo);
	klass->setMethod(1, fooMethod);
}