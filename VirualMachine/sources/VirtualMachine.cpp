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

using namespace Beer;


//#define BEER_VM_VERBOSE

#ifdef BEER_VM_VERBOSE
#	define VM_DEBUG(msg) cout << "// VM: " << msg << std::endl;
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

StackFrame* VirtualMachine::openStackFrame(Object* receiver, const char_t* selector)
{
	NULL_ASSERT(receiver);

	ClassReflection* klass = getClassTable()->translate(receiver);
	MethodReflection* method = klass->findMethod(selector);
	
	if(method == NULL)
	{
		throw MethodNotFoundException(string(klass->getName()) + BEER_WIDEN(" has no method ") + selector);
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
	RUNTIME_ASSERT(it == mClasses.end(), string(BEER_WIDEN("Class with such name already exists: ")) + reflection->getName());
	mClasses[reflection->getName()] = reflection;
	VM_DEBUG(BEER_WIDEN("Added class ") << reflection->getName());
}

void VirtualMachine::removeClass(ClassReflection* reflection)
{
	ClassReflectionTable::iterator it = mClasses.find(reflection->getName());
	RUNTIME_ASSERT(it != mClasses.end(), BEER_WIDEN("No class with such name"));
	mClasses.erase(it);
	VM_DEBUG(BEER_WIDEN("Removed class ") << reflection->getName());
}

bool VirtualMachine::hasClass(string name) const
{
	return (mClasses.find(name) != mClasses.end()) || mClassLoader->canLoadClass(name);
}
	
ClassReflection* VirtualMachine::getClass(string name)
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
	mClassHeap = new MarkAndSweepGC();

	mDebugger = new Debugger(this);
	mClassLoader = new ClassLoader(this, mClassHeap);
	mStack = new WorkStack(stackInitSize);
	mFrames = new Frames();

	mHeap = new CopyGC(this, mStack, heapInitSize);

	mClassLoader->addClassInitializer(BEER_WIDEN("Object"), new ObjectClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("String"), new StringClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Boolean"), new BooleanClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Character"), new CharacterClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Integer"), new IntegerClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Float"), new FloatClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Console"), new ConsoleClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Array"), new ArrayClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Timer"), new TimerClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("FileReader"), new FileReaderClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("FileWriter"), new FileWriterClassInitializer);
	//mClassLoader->addClassInitializer(BEER_WIDEN("Main"), new MainClassInitializer);

	// preloading of some classes
	mObjectClass = getClass(BEER_WIDEN("Object"));
	mStringClass = getClass(BEER_WIDEN("String"));
	mFloatClass = getClass(BEER_WIDEN("Float"));
	mCharacterClass = getClass(BEER_WIDEN("Character"));
	mBooleanClass = getClass(BEER_WIDEN("Boolean"));
	mIntegerClass = getClass(BEER_WIDEN("Integer"));

	// !!! the order is important !!!
	////////////////////////////////////////////// xxxxxxx0 // Object
	getClassTable()->add(mIntegerClass);		// xxxxxx01 // Integer
	getClassTable()->add(mBooleanClass);		// xxxx0011 // Boolean
	getClassTable()->add(mCharacterClass);		// xxxx0111 // Character
	
	// other
#ifdef BEER_INLINE_OPTIMALIZATION
	mInlineFnTable.fill();
#endif // BEER_INLINE_OPTIMALIZATION
	
	// prepares stack
	//Object* main = getClass(BEER_WIDEN("Main"))->createInstance(NULL, getHeap());
	//mStack->push(main);
	openStackFrame()->done = true;
}

void VirtualMachine::destroy()
{
	//RUNTIME_ASSERT(getStack()->empty(), BEER_WIDEN("Stack is not empty!"));
	//RUNTIME_ASSERT(Instance::gPool.empty(), BEER_WIDEN("Some instances were not deleted!"));
	
	// TODO: unload classes
	mClasses.clear();

	SMART_DELETE(mHeap);
	SMART_DELETE(mFrames);
	SMART_DELETE(mStack);
	SMART_DELETE(mClassLoader);
	SMART_DELETE(mClassHeap);
}

void VirtualMachine::invoke(StackFrame* frame)
{
	NULL_ASSERT(frame);
	NULL_ASSERT(frame->method);
	
	frame->method->call(this, frame);
}

void VirtualMachine::run()
{
	Object* main = getClass(BEER_WIDEN("Main"))->createInstance(getStackFrame(), getHeap());
	mStack->push(main);
	openStackFrame(main, BEER_WIDEN("Main::run()"));

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

		Console::getOutput().flush(cout); // DBG ONLY
	}

	getDebugger()->ended();

	// flush all output
	Console::getOutput().flush(cout); // TODO
}

Integer* VirtualMachine::createInteger(int32 value)
{
	if(Integer::canBeInlineValue(value))
	{
		return Integer::makeInlineValue(value);
	}
	
	Integer* object = getIntegerClass()->createInstance<Integer>(getStackFrame(), getHeap());
	object->setNonInlineValue(value);
	return object;
}

Float* VirtualMachine::createFloat(Float::FloatData value)
{
	Float* object = getFloatClass()->createInstance<Float>(getStackFrame(), getHeap());
	object->setData(value);
	return object;
}

String* VirtualMachine::createString(const Character::CharacterData* value)
{
	String* object = createString(strlen(value));
	object->copyData(value);
	return object;
}

String* VirtualMachine::createString(const string& s)
{
	getStackFrame()->stackPush(createInteger(s.size())); // TODO
	String* object = getStringClass()->createInstance<String>(getStackFrame(), getHeap());
	object->copyData(s.c_str());
	return object;
}

String* VirtualMachine::createString(String::LengthData length)
{
	getStackFrame()->stackPush(createInteger(length)); // TODO
	String* object = getStringClass()->createInstance<String>(getStackFrame(), getHeap());
	return object;
}