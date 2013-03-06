#include "stdafx.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "CopyGC.h"
#include "ClassLoader.h"

#include "Class.h"
#include "MetaClass.h"
#include "Method.h"

#include "Object.h"
#include "String.h"
#include "Boolean.h"
#include "Integer.h"
#include "Float.h"
#include "Console.h"
#include "Array.h"
#include "Timer.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Task.h"
#include "LoadedObject.h"
#include "Pair.h"

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
	//mStack = new WorkStack(stackInitSize);
	mHeap = new CopyGC(this, heapInitSize);
	mClassLoader = new ClassLoader(this, mHeap);

	Bytecode::init(this);

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
	mMetaClass->mFlags = 0;
	mMetaClass->mParentsCount = 1;
	mMetaClass->mPropertiesCount = 0;
	mMetaClass->mMethodsCount = 2;
	mMetaClass->setName(metaClassName);

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

	mObjectClass = mClassLoader->createClass<Class>(objectClassName, 1, 0, 1); // extends Object, 1 method
	// TODO: default methods
	
	// fix references
	mMetaClass->extends(0, mObjectClass); // TODO
	mObjectClass->extends(0, mObjectClass); // TODO

	// create String class
	String* stringClassName = ((GarbageCollector*)mHeap)->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "String", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	stringClassName->size(6); // 6 for "String"
	stringClassName->copyData(BEER_WIDEN("String"));

	mStringClass = mClassLoader->createClass<Class>(stringClassName, 1, 0, 16);
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

	// default Object methods
	Method* objectCreateInstance = mClassLoader->createMethod(1, 0);
	objectCreateInstance->setFunction(&Class::createInstance);
	objectCreateInstance->setName(createString(BEER_WIDEN("createInstance")));
	mObjectClass->setMethod(0, createPair(createString(BEER_WIDEN("$Class::createInstance()")), objectCreateInstance));

	// preloading of some classes
	mPairClass = getClass(BEER_WIDEN("Pair"));
	mFloatClass = getClass(BEER_WIDEN("Float"));
	mBooleanClass = getClass(BEER_WIDEN("Boolean"));
	mIntegerClass = getClass(BEER_WIDEN("Integer"));
	mArrayClass = getClass(BEER_WIDEN("Array"));
	//mTaskClass = getClass(BEER_WIDEN("Task"));

	// !!! the order is important !!!
	////////////////////////////////////////////////////// xxxxxxx0 // Object
	getClassTable()->add(mIntegerClass);							// xxxxxx01 // Integer
	getClassTable()->add(mBooleanClass);							// xxxx0011 // Boolean
	getClassTable()->add(getClass(BEER_WIDEN("Character")));		// xxxx0111 // Character
	
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
	SMART_DELETE(mClassLoader);
}

void VirtualMachine::work()
{
	StackFrame* frame = getStackFrame();
	StackRef<Class> entryPointClass(frame, frame->stackPush(getClass(BEER_WIDEN("EntryPoint")))); // TODO

	if(entryPointClass.isNull())
	{
		throw ClassNotFoundException(BEER_WIDEN("Class EntryPoint not found"));
	}

	for(ClassReflectionTable::iterator it = mClasses.begin(); it != mClasses.end(); it++)
	{
		StackRef<Class> klass(frame, frame->stackPush(it->second)); // TODO
		//cout << BEER_WIDEN("Class: ") << klass->getName()->c_str() << BEER_WIDEN("\n");

		if(klass.get() != entryPointClass.get() && klass->substituable(*entryPointClass))  // TODO
		{
			StackRef<Object> instance(frame, frame->stackPush(NULL));

			// new Main
			{
				StackRef<String> selector(frame, frame->stackPush(createString(BEER_WIDEN("$Class::createInstance()")))); // TODO
				StackRef<Method> method(frame, frame->stackPush(klass->findMethod(*selector))); // TODO
				if(method.isNull())
				{
					throw MethodNotFoundException(*klass, klass->getClass<Class>(), *selector); // TODO
				}

				StackRef<Object> ret(frame, frame->stackPush(NULL));
				frame->stackPush(*klass);

				StackFrame* nextFrame = openStackFrame();
				method->call(this); // pops copied mainClass
				closeStackFrame();

				instance = ret;

				frame->stackMoveTop(-3);
			}

			// Main::Main()
			{
				StackRef<String> selector(frame, frame->stackPush(createString(BEER_WIDEN("Main::Main()")))); // TODO
				StackRef<Method> method(frame, frame->stackPush(klass->findMethod(*selector))); // TODO

				if(!method.isNull())
				{
					// ugly – create another trampoline
					TrampolineThread* thread = new TrampolineThread(this);
					getThreads().insert(thread);

					thread->openStackFrame()->stackMoveTop(1); // for return
					thread->getStackFrame()->stackPush(*instance); // push receiver
					thread->openStackFrame()->method = *method;

					thread->run();
					thread->wait();

					instance = thread->getStack()->top(0); // fix main // TODO
				}

				frame->stackMoveTop(-2); // pop method & selector
			}

			// Task::run()
			{
				frame = openStackFrame();
				frame->stackPush(*instance);

				frame = openStackFrame();

				StackRef<String> selector(frame, frame->stackPush(createString(BEER_WIDEN("Task::dorun()")))); // TODO
				StackRef<Method> method(frame, frame->stackPush(klass->findMethod(*selector))); // TODO

				if(method.isNull())
				{
					throw MethodNotFoundException(*instance, *klass, *selector); // TODO
				}

				frame->method = *method;
				frame->stackMoveTop(-2); // pop method & selector
				frame->method->call(this);

				closeStackFrame();
			}

			frame->stackMoveTop(-1); // pop instance
		}

		frame->stackMoveTop(-1); // pop class
	}

	frame->stackMoveTop(-1); // pop entrypoint class
	
	// wait for all threads
	for(ThreadSet::iterator it = mThreads.begin(); it != mThreads.end(); it++)
	{
		Thread* thread = *it;
		if(thread != static_cast<Thread*>(this))
		{
			(*it)->wait();
		}
	}
	
	
	DBG_ASSERT(frame->stackSize() == 0, BEER_WIDEN("Stack was not properly cleaned"));
}

String* VirtualMachine::createString(const string& s)
{
	StackFrame* frame = getStackFrame();
	
	StackRef<String> stringOnStack(frame, frame->stackPush(NULL));
	
	StackRef<Integer> lengthOnStack(frame, frame->stackPush(NULL));
	createInteger(lengthOnStack, s.size());

	// TODO: why the cast???
	((Thread*)this)->createString(lengthOnStack, stringOnStack);

	//String* object = getStringClass()->createInstance<String>(this, frame, getHeap()); // pops length
	String* object = *stringOnStack;

	frame->stackMoveTop(-1); // pop string
	object->copyData(s.c_str());

	return object;
}

Pair* VirtualMachine::createPair(Object* first, Object* second)
{
	StackFrame* frame = getStackFrame();

	StackRef<Pair> ret(frame, frame->stackPush());
	StackRef<Object> arg2(frame, frame->stackPush(second));
	StackRef<Object> arg1(frame, frame->stackPush(first));

	// TODO: why the cast???
	((Thread*)this)->createPair(arg1, arg2, ret);

	Pair* p = *ret;
	frame->stackMoveTop(-1);
	return p;
}