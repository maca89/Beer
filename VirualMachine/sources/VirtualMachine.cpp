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
#include "TaskClass.h"
#include "LoadedObject.h"

using namespace Beer;


//#define BEER_VM_VERBOSE

#ifdef BEER_VM_VERBOSE
#	define VM_DEBUG(msg) cout << "// VM: " << msg << std::endl;
#else
#	define VM_DEBUG(msg)
#endif

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
	mClassHeap = new MarkAndSweepGC(); // TODO: get rid of this
	mDebugger = new Debugger(this);
	mClassLoader = new ClassLoader(this, mClassHeap);
	mStack = new WorkStack(stackInitSize);

	mHeap = new CopyGC(this, heapInitSize);

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
	mClassLoader->addClassInitializer(BEER_WIDEN("Task"), new TaskClassInitializer);
	//mClassLoader->addClassInitializer(BEER_WIDEN("Main"), new MainClassInitializer);

	// preloading of some classes
	mObjectClass = getClass(BEER_WIDEN("Object"));
	mStringClass = getClass(BEER_WIDEN("String"));
	mFloatClass = getClass(BEER_WIDEN("Float"));
	mCharacterClass = getClass(BEER_WIDEN("Character"));
	mBooleanClass = getClass(BEER_WIDEN("Boolean"));
	mIntegerClass = getClass(BEER_WIDEN("Integer"));
	mTaskClass = getClass(BEER_WIDEN("Task"));

	// !!! the order is important !!!
	////////////////////////////////////////////// xxxxxxx0 // Object
	getClassTable()->add(mIntegerClass);		// xxxxxx01 // Integer
	getClassTable()->add(mBooleanClass);		// xxxx0011 // Boolean
	getClassTable()->add(mCharacterClass);		// xxxx0111 // Character
	
	// other
#ifdef BEER_INLINE_OPTIMALIZATION
	mInlineFnTable.fill();
#endif // BEER_INLINE_OPTIMALIZATION
}

void VirtualMachine::destroy()
{
	// TODO: unload classes
	mClasses.clear();

	SMART_DELETE(mHeap);
	SMART_DELETE(mStack);
	SMART_DELETE(mClassLoader);
	SMART_DELETE(mClassHeap);
}

void VirtualMachine::run()
{
	Frames frames;
	StackFrame* frame = NULL;

	frames.push_back(StackFrame(mStack));
	frame = &frames.back();
	Object* main = getClass(BEER_WIDEN("Main"))->createInstance(this, frame, getHeap());
 	frame->stackPush(main); // push receiver

	frames.push_back(StackFrame(frame));
	frame = &frames.back();
	
	//openStackFrame(main, BEER_WIDEN("Main::run()"));

	// ugly, TODO: some other way
	RUNTIME_ASSERT(main->getClass()->substituable(getTaskClass()), BEER_WIDEN("Main is not instance of Task"));
	frame->method = main->getClass()->findMethod(BEER_WIDEN("Task::dorun()"));
	NULL_ASSERT(frame->method);
	frame->method->call(this, frame);
}

Integer* VirtualMachine::createInteger(Integer::IntegerData value)
{
	if(Integer::canBeInlineValue(value))
	{
		return Integer::makeInlineValue(value);
	}
	
	Integer* object = getIntegerClass()->createInstance<Integer>(this, NULL, getHeap());
	object->setNonInlineValue(value);
	return object;
}

Float* VirtualMachine::createFloat(Float::FloatData value)
{
	Float* object = getFloatClass()->createInstance<Float>(this, NULL, getHeap());
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
	StackFrame frame(mStack); // TODO
	frame.stackPush(createInteger(s.size())); // TODO
	String* object = getStringClass()->createInstance<String>(this, &frame, getHeap());
	object->copyData(s.c_str());
	return object;
}

String* VirtualMachine::createString(String::LengthData length)
{
	StackFrame frame(mStack); // TODO
	frame.stackPush(createInteger(length)); // TODO
	String* object = getStringClass()->createInstance<String>(this, &frame, getHeap());
	return object;
}