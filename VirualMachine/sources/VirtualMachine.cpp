#include "stdafx.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "GenerationalGC.h"
#include "ClassFileLoader.h"
#include "ClassLoader.h"
#include "BytecodeBuilder.h"

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
#include "CreateOneEntryPointTask.h"
#include "CreateAllEntryPointsTask.h"
#include "LoadedObject.h"
#include "Pair.h"
#include "RandomGenerator.h"
#include "Pool.h"

using namespace Beer;


//#define BEER_VM_VERBOSE

#ifdef BEER_VM_VERBOSE
#	define VM_DEBUG(msg) cout << "// VM: " << msg << std::endl;
#else
#	define VM_DEBUG(msg)
#endif


void VirtualMachine::addClass(Thread* thread, Class* klass)
{
	Frame* frame = thread->getFrame();
	String* name = NULL;

	// get name, TODO
	{
		StackRef<String> nameOnStack(frame, frame->stackPush());
		StackRef<Class> klass(frame, frame->stackPush(klass));
		Class::getName(thread, klass, nameOnStack);
		name = *nameOnStack;
		frame->stackMoveTop(-2); // pop class & name
	}

	ClassReflectionTable::iterator it = mClasses.find(name->c_str()); // TODO
	RUNTIME_ASSERT(it == mClasses.end(), string(BEER_WIDEN("Class with such name already exists: ")) + name->c_str()); // TODO
	mClasses[name->c_str()] = klass; // TODO
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
	
Class* VirtualMachine::findClass(StackRef<String> name)
{
	ClassReflectionTable::iterator it = mClasses.find(name->c_str()); // TODO
	if(it != mClasses.end()) return mClasses.find(name->c_str())->second; // TODO

	mClassLoader->loadClass(this, name);

	// TODO
	it = mClasses.find(name->c_str()); // TODO
	if(it != mClasses.end()) return mClasses.find(name->c_str())->second; // TODO

	throw ClassNotFoundException(name->c_str()); // TODO
}

void VirtualMachine::loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile)
{
	this->Thread::loadClassFile(loader, classFile);
}

void VirtualMachine::run()
{
	this->Thread::run();
}

void VirtualMachine::wait()
{
	this->Thread::wait();
}

void VirtualMachine::init()
{
	this->Thread::init();
	//mGC = new GenerationalGC(27, 32768);
	//mGC->init();

	mDebugger = new Debugger(this, mGC);
	mDebugger->init();

	mClassLoader = new ClassLoader(); 

	mBytecodeBuilder = new BytecodeBuilder();
	Bytecode::init(this);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// init objects and their classes
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		Frame* frame = getFrame();
		BEER_STACK_CHECK();

		// create MetaClass
		StackRef<MetaClass> metaClass(frame, frame->stackPush(
			mMetaClass = getPermanentHeap()->alloc<MetaClass>(
				Class::CLASS_CHILDREN_COUNT + 1 + 2 + Object::OBJECT_METHODS_COUNT // 1 parent, 2 methods
		)));

		mMetaClass = *metaClass; // ugly, TODO
		mMetaClass->mFlags = 0; // deprecated
		mMetaClass->mParentsCount = 1;
		mMetaClass->mPropertiesCount = 0;
		mMetaClass->mMethodsCount = 2 + Object::OBJECT_METHODS_COUNT;
		mMetaClass->mParentNext = mMetaClass->mMethodNext = mMetaClass->mPropertyNext = 0;
		mMetaClass->mTraverser = &MetaClass::DefaultClassTraverser;

		// create MetaClass name
		StackRef<String> metaClassName(frame, frame->stackPush(
			mHeap->alloc<String>(
				static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (9 + 1)), // 9 for "MetaClass", 1 for "\0"
				Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
		)));

		metaClassName->size(9); // 9 for "MetaClass"
		metaClassName->copyData(BEER_WIDEN("MetaClass"));

		Object::setChild(static_cast<Thread*>(this), metaClass, Class::CHILD_ID_CLASS_NAME, metaClassName); // set name

		mClassLoader->addClassInitializer(BEER_WIDEN("Boolean"), new BooleanClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Character"), new CharacterClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Integer"), new IntegerClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Float"), new FloatClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Console"), new ConsoleClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Array"), new ArrayClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Timer"), new TimerClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("FileReader"), new FileReaderClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("FileWriter"), new FileWriterClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Pair"), new PairClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("RandomGenerator"), new RandomGeneratorClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Pool"), new PoolClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Task"), new TaskInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("CreateOneEntryPointTask"), new CreateOneEntryPointTaskInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("CreateAllEntryPointsTask"), new CreateAllEntryPointsTaskInitializer);

		// create Object class name
		StackRef<String> objectClassName(frame, frame->stackPush(
			mHeap->alloc<String>(
				static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Object", 1 for "\0"
				Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
		)));
		objectClassName->size(6); // 6 for "Object"
		objectClassName->copyData(BEER_WIDEN("Object"));

		// create Method class name
		StackRef<String> methodClassName(frame, frame->stackPush(
			mHeap->alloc<String>(
				static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Method", 1 for "\0"
				Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
		)));
		methodClassName->size(6); // 6 for "Method"
		methodClassName->copyData(BEER_WIDEN("Method"));

		// vars
		StackRef<Class> objectClass(frame, frame->stackPush());
		StackRef<Class> methodClass(frame, frame->stackPush());

		// create Method class
		{
			mClassLoader->createClass<Class>(this, methodClassName, methodClass, 1, 0, Object::OBJECT_METHODS_COUNT + 6); // extends Object, has 6 methods
			methodClass->setTraverser(&Method::MethodTraverser);
			mMethodClass = *methodClass;
		}

		// create Object class
		{
			mClassLoader->createClass<Class>(this, objectClassName, objectClass, 1, 0, Object::OBJECT_METHODS_COUNT); // extends Object, has 6 methods
			mObjectClass = *objectClass;
		}

		// create String class
		{
			StackRef<String> stringClassName(frame, frame->stackPush(
				mHeap->alloc<String>(
					static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "String", 1 for "\0"
					Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
			)));
			stringClassName->size(6); // 6 for "String"
			stringClassName->copyData(BEER_WIDEN("String"));

			StackRef<Class> stringClass(frame, frame->stackPush());
			mClassLoader->createClass<Class>(this, stringClassName, stringClass, 1, 0, 17 + Object::OBJECT_METHODS_COUNT);

			mStringClass = *stringClass; // ugly, TODO

			StringClassInitializer* stringInit = new StringClassInitializer();
			stringInit->initClass(this, mClassLoader, stringClass);
			delete stringInit;

			// fix references
			Object::setType(this, metaClassName, stringClass); // set class
			Object::setType(this, objectClassName, stringClass); // set class
			Object::setType(this, stringClassName, stringClass); // set class

			frame->stackMoveTop(-2); // pop stringClassName, stringClass
		}

		// fix metaclass methods
		mClassLoader->addMethod(this, metaClass.staticCast<Class>(), BEER_WIDEN("MetaClass"), BEER_WIDEN("MetaClass::MetaClass()"), &MetaClass::init, 1, 0);
		//mClassLoader->addMethod(*(metaClass.staticCast<Class>()), BEER_WIDEN("findMethod"), BEER_WIDEN("Class::findMethod(String)"), &MetaClass::findMethod, 1, 1);

		// default Object methods
		mClassLoader->addMethod(this, objectClass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Class::createInstance, 1, 0);
		mClassLoader->addMethod(this, objectClass, BEER_WIDEN("Object"), BEER_WIDEN("Object::Object()"), &Object::init, 1, 0);
		mClassLoader->addMethod(this, objectClass, BEER_WIDEN("String"), BEER_WIDEN("Object::String()"), &Object::operatorString, 1, 0);
		// TODO: getType

		// fix references
		Class::addParent(this, metaClass.staticCast<Class>(), objectClass);
		Class::addParent(this, objectClass, objectClass);
		Class::addParent(this, methodClass, objectClass);

		// TODO: fix Method methods

		// create Task class
		//findClass(BEER_WIDEN("Task"));
		//findClass(BEER_WIDEN("Task"));


		frame->stackMoveTop(-6); // clean
		//closeFrame();
	}

	// preloading of some classes
	mFloatClass = findClass(BEER_WIDEN("Float"));
	mBooleanClass = findClass(BEER_WIDEN("Boolean"));
	mIntegerClass = findClass(BEER_WIDEN("Integer"));
	mArrayClass = findClass(BEER_WIDEN("Array"));
	//mTaskClass = findClass(BEER_WIDEN("Task"));
	mPoolClass = findClass(BEER_WIDEN("Pool"));

	// !!! the order is important !!!
	////////////////////////////////////////////////////// xxxxxxx0 // Object
	getClassTable()->add(mIntegerClass);							// xxxxxx01 // Integer
	getClassTable()->add(mBooleanClass);							// xxxx0011 // Boolean
	getClassTable()->add(findClass(BEER_WIDEN("Character")));		// xxxx0111 // Character
	
	// other
#ifdef BEER_INLINE_OPTIMALIZATION
	mInlineFnTable.fill();
#endif // BEER_INLINE_OPTIMALIZATION
}

void VirtualMachine::destroy()
{
	// TODO: unload classes
	mClasses.clear();

	//SMART_DELETE(mHeap);
	SMART_DELETE(mClassLoader);
}

void VirtualMachine::work()
{
	CreateAllEntryPointsTask* task = mHeap->alloc<CreateAllEntryPointsTask>(CreateAllEntryPointsTask::CREATEEPSTASK_CHILDREN_COUNT);
	task->setType(findClass(BEER_WIDEN("CreateAllEntryPointsTask")));

	// TODO: schedule

	// temporary solution
	{
		Frame* frame = getFrame();
		BEER_STACK_CHECK();
	
		StackRef<CreateAllEntryPointsTask> taskOnStack(frame, frame->stackPush(task));
		CreateAllEntryPointsTask::work(this, taskOnStack);

		frame->stackPop(); // pop taskOnStack
	}

	// wait for all threads
	for(ThreadSet::iterator it = mThreads.begin(); it != mThreads.end(); it++)
	{
		Thread* thread = *it;
		if(thread != static_cast<Thread*>(this))
		{
			(*it)->wait();
		}
	}
	
	//DBG_ASSERT(frame->stackLength() == 2, BEER_WIDEN("Stack was not properly cleaned"));
}

String* VirtualMachine::createString(const string& s)
{
	Frame* frame = getFrame();
	
	StackRef<String> stringOnStack(frame, frame->stackPush(NULL));
	
	StackRef<Integer> lengthOnStack(frame, frame->stackPush(NULL));
	createInteger(lengthOnStack, s.size());

	// TODO: why the cast???
	((Thread*)this)->createString(lengthOnStack, stringOnStack);

	//String* object = getStringClass()->createInstance<String>(this, frame, getHeap()); // initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass) length
	String* object = *stringOnStack;

	frame->stackPop(); // pop string
	object->copyData(s.c_str());

	return object;
}

Pair* VirtualMachine::createPair(Object* first, Object* second)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Pair> ret(frame, frame->stackPush());
	StackRef<Object> arg2(frame, frame->stackPush(second));
	StackRef<Object> arg1(frame, frame->stackPush(first));

	// TODO: why the cast???
	((Thread*)this)->createPair(arg1, arg2, ret);

	Pair* p = *ret;
	frame->stackPop();
	return p;
}

Class* VirtualMachine::findClass(string name)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<String> nameOnStack(frame, frame->stackPush());
	((Thread*)this)->createString(nameOnStack, name);
	
	Class* c = findClass(nameOnStack);
	frame->stackPop(); // pop nameOnStack
	return c;
}