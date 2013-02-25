#include "stdafx.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "CopyGC.h"
#include "ClassLoader.h"

#include "Class.h"
#include "MetaClass.h"
#include "Method.h"

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
#include "Task.h"
#include "LoadedObject.h"
#include "PairClass.h"

using namespace Beer;


//#define BEER_VM_VERBOSE

#ifdef BEER_VM_VERBOSE
#	define VM_DEBUG(msg) cout << "// VM: " << msg << std::endl;
#else
#	define VM_DEBUG(msg)
#endif

void VirtualMachine::addClass(Class* reflection)
{
	ClassReflectionTable::iterator it = mClasses.find(reflection->getName()->c_str()); // TODO
	RUNTIME_ASSERT(it == mClasses.end(), string(BEER_WIDEN("Class with such name already exists: ")) + reflection->getName()->c_str()); // TODO
	mClasses[reflection->getName()->c_str()] = reflection; // TODO
	VM_DEBUG(BEER_WIDEN("Added class ") << reflection->getName());
}

/*void VirtualMachine::removeClass(ClassReflection* reflection)
{
	throw Exception(BEER_WIDEN("Not yet implemented"), __WFILE__, __LINE__);

	// TODO
	ClassReflectionTable::iterator it = mClasses.find(reflection->getName()->c_str());
	RUNTIME_ASSERT(it != mClasses.end(), BEER_WIDEN("No class with such name"));
	mClasses.erase(it);
	VM_DEBUG(BEER_WIDEN("Removed class ") << reflection->getName());
}

bool VirtualMachine::hasClass(string name) const
{
	// TODO
	return (mClasses.find(name) != mClasses.end()) || mClassLoader->canLoadClass(name);
}*/
	
Class* VirtualMachine::getClass(String* name)
{
	ClassReflectionTable::iterator it = mClasses.find(name->c_str()); // TODO
	if(it != mClasses.end()) return mClasses.find(name->c_str())->second; // TODO

	mClassLoader->loadClass(name);

	// TODO
	it = mClasses.find(name->c_str()); // TODO
	if(it != mClasses.end()) return mClasses.find(name->c_str())->second; // TODO

	throw ClassNotFoundException(name->c_str()); // TODO
}

void VirtualMachine::init(uint32 stackInitSize, uint32 heapInitSize)
{
	mDebugger = new Debugger(this);
	mStack = new WorkStack(stackInitSize);
	mHeap = new CopyGC(this, heapInitSize);
	mClassLoader = new ClassLoader(this, mHeap);

	
	String* metaClassName = ((GarbageCollector*)mHeap)->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (9 + 1)), // 10 for "MetaClass", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	metaClassName->size(9); // 9 for "MetaClass"
	metaClassName->copyData(BEER_WIDEN("MetaClass"));

	mMetaClass = static_cast<GarbageCollector*>(mHeap)->alloc<MetaClass>(
		Class::CLASS_CHILDREN_COUNT + 3 // 1 parent, 2 methods
	);

	new(mMetaClass) MetaClass(); // init v_table, TODO: get rid of
	mMetaClass->setName(metaClassName);
	mMetaClass->mFlags = 0;
	mMetaClass->mParentsCount = 1;
	mMetaClass->mPropertiesCount = 0;
	mMetaClass->mMethodsCount = 2;

	Method* metaClassCtor = mClassLoader->createMethod(1, 0);
	metaClassCtor->setFunction(&MetaClass::init);

	Method* metaClassFindMethod = mClassLoader->createMethod(1, 1);
	metaClassFindMethod->setFunction(&MetaClass::findMethod);

	mClassLoader->addClassInitializer(BEER_WIDEN("Boolean"), new BooleanClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Character"), new CharacterClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Integer"), new IntegerClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Float"), new FloatClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Console"), new ConsoleClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Array"), new ArrayClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Timer"), new TimerClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("FileReader"), new FileReaderClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("FileWriter"), new FileWriterClassInitializer);
	//mClassLoader->addClassInitializer(BEER_WIDEN("Task"), new TaskClassInitializer);
	mClassLoader->addClassInitializer(BEER_WIDEN("Pair"), new PairClassInitializer);

	// create Object class
	String* objectClassName = ((GarbageCollector*)mHeap)->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Object", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	objectClassName->size(6); // 6 for "Object"
	objectClassName->copyData(BEER_WIDEN("Object"));

	mObjectClass = mClassLoader->createClass<ObjectClass>(objectClassName, 0, 0, 0);
	// TODO: default methods
	
	// fix references
	mMetaClass->extends(0, mObjectClass); // TODO

	// create String class
	String* stringClassName = ((GarbageCollector*)mHeap)->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "String", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	objectClassName->size(6); // 6 for "String"
	objectClassName->copyData(BEER_WIDEN("String"));

	mStringClass = mClassLoader->createClass<StringClass>(stringClassName, 1, 0, 15);
	StringClassInitializer* stringInit = new StringClassInitializer();
	stringInit->initClass(this, mClassLoader, mStringClass);
	delete stringInit;

	// fix references
	objectClassName->setClass(mStringClass); // important!
	stringClassName->setClass(mStringClass); // important!


	// fix metaclass methods
	metaClassCtor->setName(createString(BEER_WIDEN("MetaClass")));
	mMetaClass->setMethod(0, createPair(createString(BEER_WIDEN("MetaClass::MetaClass()")), metaClassCtor));

	metaClassFindMethod->setName(createString(BEER_WIDEN("open")));
	mMetaClass->setMethod(1, createPair(createString(BEER_WIDEN("MetaClass::findMethod(String)")), metaClassFindMethod));

	// preloading of some classes
	mPairClass = getClass(BEER_WIDEN("Pair"));
	mFloatClass = getClass(BEER_WIDEN("Float"));
	mCharacterClass = getClass(BEER_WIDEN("Character"));
	mBooleanClass = getClass(BEER_WIDEN("Boolean"));
	mIntegerClass = getClass(BEER_WIDEN("Integer"));
	//mTaskClass = getClass(BEER_WIDEN("Task"));

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
	typedef std::list<StackFrame> Frames;
	Frames frames;
	StackFrame* frame = NULL;

	frames.push_back(StackFrame(mStack));
	frame = &frames.back();

	StackRef<Object> main = StackRef<Object>(frame, frame->stackPush(getClass(BEER_WIDEN("Main"))->createInstance(this, frame, getHeap())));
	Class* mainClass = main->getClass<Class>();

	// call constructor
	Method* mainCtor = NULL;
	String* selector = createString(BEER_WIDEN("Main::Main()")); // TODO
	if(mainCtor = mainClass->findMethod(selector))
	{
		// ugly – create another trampoline
		TrampolineThread* thread = new TrampolineThread(this);
		getThreads().insert(thread);

		thread->openStackFrame()->stackMoveTop(1); // for return
		thread->getStackFrame()->stackPush(*main); // push receiver
		thread->openStackFrame()->method = mainCtor;

		thread->run();
		thread->wait();

		main = thread->getStack()->top(0); // fix main // TODO
	}

	//openStackFrame(main, BEER_WIDEN("Main::run()"));

	// ugly, TODO: some other way
	//RUNTIME_ASSERT(mainClass->substituable(getTaskClass()), BEER_WIDEN("Main is not instance of Task"));


	// push receiver
	frames.push_back(StackFrame(frame));
	frame = &frames.back();
	frame->stackPush(*main);

	frames.push_back(StackFrame(frame));
	frame = &frames.back();
	selector = createString(BEER_WIDEN("Task::dorun()")); // TODO
	frame->method = mainClass->findMethod(selector);
	
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

Pair* VirtualMachine::createPair(Object* first, Object* second)
{
	StackFrame frame(mStack);
	Pair* pair = getPairClass()->createInstance<Pair>(this, &frame, getHeap());

	// TODO: call ctor
	pair->setFirst(first);
	DBG_ASSERT(pair->getFirst<Object>() == first, BEER_WIDEN("Object saved into Pair differs from the given"));

	pair->setSecond(second);
	DBG_ASSERT(pair->getSecond<Object>() == second, BEER_WIDEN("Object saved into Pair differs from the given"));

	return pair;
}

/*void VirtualMachine::createInstance(StackFrame* frame, Class* klass)
{
	frame->stackPush(klass);
	MethodReflection* method = klass->findMethod(createString(BEER_WIDEN("Class::createInstance()")));
}*/