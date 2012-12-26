#include "stdafx.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "MarkAndSweepGC.h"
#include "CopyGC.h"
#include "ClassLoader.h"

#include "MethodReflection.h"

#include "ObjectClass.h"
#include "StringClass.h"
#include "BooleanClass.h"
#include "IntegerClass.h"
#include "FloatClass.h"
#include "ConsoleClass.h"
#include "ArrayClass.h"
#include "TimerClass.h"
#include "FileReaderClass.h"
#include "FileWriterClass.h"

#include "MainClass.h"


using namespace Beer;


//#define BEER_VM_VERBOSE

#ifdef BEER_VM_VERBOSE
#	define VM_DEBUG(msg) std::cout << "// VM: " << msg << std::endl;
#else
#	define VM_DEBUG(msg)
#endif

StackFrame* VirtualMachine::openStackFrame()
{
	StackFrame frame(mStack, mStack->size());
	mFrames->push_back(frame);
	return getStackFrame();
}

StackFrame* VirtualMachine::openStackFrame(MethodReflection* method)
{
	NULL_ASSERT(method);
	StackFrame* frame = openStackFrame();
	frame->method = method;
	return frame;
}

StackFrame* VirtualMachine::openStackFrame(Object* receiver, const char* selector)
{
	NULL_ASSERT(receiver);

	ClassReflection* klass = getClassTable()->translate(receiver);
	MethodReflection* method = klass->findMethod(selector);
	
	if(method == NULL)
	{
		throw MethodNotFoundException(std::string(klass->getName()) + " has no method " + selector);
	}

	return openStackFrame(method);
}

void VirtualMachine::closeStackFrame()
{
	mFrames->pop_back();
}

void VirtualMachine::addClass(ClassReflection* reflection)
{
	ClassReflectionTable::iterator it = mClasses.find(reflection->getName());
	RUNTIME_ASSERT(it == mClasses.end(), std::string("Class with such name already exists: ") + reflection->getName());
	mClasses[reflection->getName()] = reflection;
	VM_DEBUG("Added class " << reflection->getName());
}

void VirtualMachine::removeClass(ClassReflection* reflection)
{
	ClassReflectionTable::iterator it = mClasses.find(reflection->getName());
	RUNTIME_ASSERT(it != mClasses.end(), "No class with such name");
	mClasses.erase(it);
	VM_DEBUG("Removed class " << reflection->getName());
}

bool VirtualMachine::hasClass(std::string name) const
{
	return (mClasses.find(name) != mClasses.end()) || mClassLoader->canLoadClass(name);
}
	
ClassReflection* VirtualMachine::getClass(std::string name)
{
	ClassReflectionTable::iterator it = mClasses.find(name);
	if(it != mClasses.end()) return mClasses.find(name)->second;

	mClassLoader->loadClass(name);

	it = mClasses.find(name);
	if(it != mClasses.end()) return mClasses.find(name)->second;

	throw ClassNotFoundException(name);
}

void VirtualMachine::init(uint32 stackInitSize, uint32 heapInitSize)
{
	mDebugger = new Debugger(this);
	mClassLoader = new ClassLoader(this);
	mStack = new WorkStack(stackInitSize);
	mFrames = new Frames();

	//mClassHeap = new MarkAndSweepGC();
	mHeap = new CopyGC(this, mStack, heapInitSize);

	mClassLoader->addClassInitializer("Object", new ObjectClassInitializer);
	mClassLoader->addClassInitializer("String", new StringClassInitializer);
	mClassLoader->addClassInitializer("Boolean", new BooleanClassInitializer);
	mClassLoader->addClassInitializer("Integer", new IntegerClassInitializer);
	mClassLoader->addClassInitializer("Float", new FloatClassInitializer);
	mClassLoader->addClassInitializer("Console", new ConsoleClassInitializer);
	mClassLoader->addClassInitializer("Array", new ArrayClassInitializer);
	mClassLoader->addClassInitializer("Timer", new TimerClassInitializer);
	mClassLoader->addClassInitializer("FileReader", new FileReaderClassInitializer);
	mClassLoader->addClassInitializer("FileWriter", new FileWriterClassInitializer);
	//mClassLoader->addClassInitializer("Main", new MainClassInitializer);

	mIntegerClassId = getClassTable()->add(getClass("Integer"));
	//mFloatClassId = getClassTable()->add(getClass("Float"));
	//mBooleanClassId = getClassTable()->add(getClass("Boolean"));
	//mCharacterClassId = getClassTable()->add(getClass("Character")); // TODO
}

void VirtualMachine::destroy()
{
	//RUNTIME_ASSERT(getStack()->empty(), "Stack is not empty!");
	//RUNTIME_ASSERT(Instance::gPool.empty(), "Some instances were not deleted!");
	
	// TODO: unload classes
	mClasses.clear();

	SMART_DELETE(mHeap);
	SMART_DELETE(mFrames);
	SMART_DELETE(mStack);
	SMART_DELETE(mClassLoader);
}

void VirtualMachine::invoke(StackFrame* frame)
{
	NULL_ASSERT(frame);
	NULL_ASSERT(frame->method);
	
	frame->method->call(this, frame);
}

void VirtualMachine::run()
{
	Main* main = getClass("Main")->createInstance<Main>(NULL, getHeap());
	mStack->push(main);
	openStackFrame(main, "Main::run()");

	getDebugger()->started();

	while(hasStackFrame())
	{
		StackFrame* frame = getStackFrame();
		
		if(frame->done)
		{
			closeStackFrame();
			continue;
		}

		if(getDebugger()->isEnabled()) getDebugger()->step(frame);
		
		try
		{
			invoke(frame);
		}
		catch(Exception& ex)
		{
			if(!getDebugger()->catchException(frame, ex)) throw ex;
		}
	}

	getDebugger()->ended();

	// flush all output
	Console::getOutput().flush(std::cout); // TODO
}

Integer* VirtualMachine::createInteger(int32 value)
{
	if(Integer::canBeInlineValue(value))
	{
		return Integer::makeInlineValue(value);
	}
	
	Integer* object = getClass("Integer")->createInstance<Integer>(getStackFrame(), getHeap());
	object->setNonInlineValue(value);
	return object;
}

Float* VirtualMachine::createFloat(Float::FloatData value)
{
	Float* object = getClass("Float")->createInstance<Float>(getStackFrame(), getHeap());
	object->setData(value);
	return object;
}

Boolean* VirtualMachine::createBoolean(Boolean::BooleanData value)
{
	Boolean* object = getClass("Boolean")->createInstance<Boolean>(getStackFrame(), getHeap());
	object->setData(value);
	return object;
}

String* VirtualMachine::createString(std::string s)
{
	getStackFrame()->stackPush(createInteger(s.size())); // TODO
	String* object = getClass("String")->createInstance<String>(getStackFrame(), getHeap());
	object->copyData(s.c_str());
	return object;
}

String* VirtualMachine::createString(String::LengthData length)
{
	getStackFrame()->stackPush(createInteger(length)); // TODO
	String* object = getClass("String")->createInstance<String>(getStackFrame(), getHeap());
	return object;
}