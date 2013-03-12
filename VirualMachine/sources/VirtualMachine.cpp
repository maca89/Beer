#include "stdafx.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "GenerationalGC.h"
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
#include "RandomGenerator.h"

using namespace Beer;


//#define BEER_VM_VERBOSE

#ifdef BEER_VM_VERBOSE
#	define VM_DEBUG(msg) cout << "// VM: " << msg << std::endl;
#else
#	define VM_DEBUG(msg)
#endif

void VirtualMachine::addClass(Class* klass)
{
	StackFrame* frame = getStackFrame();
	String* name = NULL;

	// get name, TODO
	{
		StackRef<String> nameOnStack(frame, frame->stackPush());
		StackRef<Class> klass(frame, frame->stackPush(klass));
		Class::getName(this, klass, nameOnStack);
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

void VirtualMachine::init(uint32 stackInitSize)
{
	this->Thread::init();

	mDebugger = new Debugger(this);
	mClassLoader = new ClassLoader(this, mHeap);

	Bytecode::init(this);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// init objects and their classes
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		StackFrame* frame = openStackFrame();

		StackRef<Integer> childIdClass(frame, frame->stackPush());
		createInteger(childIdClass, Object::CHILD_ID_CLASS);

		StackRef<Integer> childIdClassName(frame, frame->stackPush());
		createInteger(childIdClassName, Class::CHILD_ID_CLASS_NAME);

		StackRef<String> metaClassName(frame, frame->stackPush(
			mHeap->alloc<String>(
				static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (9 + 1)), // 9 for "MetaClass", 1 for "\0"
				Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
		)));

		metaClassName->size(9); // 9 for "MetaClass"
		metaClassName->copyData(BEER_WIDEN("MetaClass"));

		StackRef<MetaClass> metaClass(frame, frame->stackPush(
			mMetaClass = mHeap->alloc<MetaClass>(
				Class::CLASS_CHILDREN_COUNT + 3 // 1 parent, 2 methods
		)));

		mMetaClass = *metaClass; // ugly, TODO
		metaClass->mFlags = 0; // deprecated
		metaClass->mParentsCount = 1;
		metaClass->mPropertiesCount = 0;
		metaClass->mMethodsCount = 2;
		metaClass->mParentNext = metaClass->mMethodNext = metaClass->mPropertyNext = 0;

		Object::setChild(static_cast<Thread*>(this), metaClass, metaClassName, childIdClassName); // set name

		mClassLoader->addClassInitializer(BEER_WIDEN("Boolean"), new BooleanClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Character"), new CharacterClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Integer"), new IntegerClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Float"), new FloatClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Console"), new ConsoleClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Array"), new ArrayClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Timer"), new TimerClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("FileReader"), new FileReaderClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("FileWriter"), new FileWriterClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Task"), new TaskInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("Pair"), new PairClassInitializer);
		mClassLoader->addClassInitializer(BEER_WIDEN("RandomGenerator"), new RandomGeneratorClassInitializer);

		// create Object class name
		StackRef<String> objectClassName(frame, frame->stackPush(
			mHeap->alloc<String>(
				static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Object", 1 for "\0"
				Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
		)));
		objectClassName->size(6); // 6 for "Object"
		objectClassName->copyData(BEER_WIDEN("Object"));

		// create Object class
		{
			StackRef<Class> objectClass(frame, frame->stackPush(
				mClassLoader->createClass<Class>(*objectClassName, 1, 0, 5) // extends Object, has 5 methods
			));

			// fix references
			mClassLoader->extendClass(metaClass.staticCast<Class>(), objectClass);
			mClassLoader->extendClass(objectClass, objectClass);

			mObjectClass = *objectClass;
			frame->stackMoveTop(-1); // pop objectClass
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

			StackRef<Class> stringClass(frame, frame->stackPush(
				mClassLoader->createClass<Class>(*stringClassName, 1, 0, 16)
			));

			mStringClass = *stringClass; // ugly, TODO

			StringClassInitializer* stringInit = new StringClassInitializer();
			stringInit->initClass(this, mClassLoader, *stringClass);
			delete stringInit;

			// fix references
			Object::setChild(static_cast<Thread*>(this), objectClassName, stringClass, childIdClass); // set class
			Object::setChild(static_cast<Thread*>(this), stringClassName, stringClass, childIdClass); // set class

			frame->stackMoveTop(-2); // pop stringClassName & stringClass
		}

		// fix metaclass methods
		mClassLoader->addMethod(*(metaClass.staticCast<Class>()), BEER_WIDEN("MetaClass"), BEER_WIDEN("MetaClass::MetaClass()"), &MetaClass::init, 1, 0);
		//mClassLoader->addMethod(*(metaClass.staticCast<Class>()), BEER_WIDEN("findMethod"), BEER_WIDEN("Class::findMethod(String)"), &MetaClass::findMethod, 1, 1);

		// default Object methods
		mClassLoader->addMethod(mObjectClass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Class::createInstance, 1, 0);

		frame->stackMoveTop(-5); // clean
		DBG_ASSERT(frame->stackSize() == 0, BEER_WIDEN("Stack was not properly cleaned"));
		closeStackFrame();
	}

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

	//SMART_DELETE(mHeap);
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
		StackRef<Boolean> substituable(frame, frame->stackPush());
		//cout << BEER_WIDEN("Class: ") << klass->getName()->c_str() << BEER_WIDEN("\n");

		Class::substituable(this, klass, entryPointClass, substituable);

		if(klass.get() != entryPointClass.get() && substituable->getData())  // TODO
		{
			StackRef<Object> instance(frame, frame->stackPush(NULL));

			// create new instance
			{
				StackRef<String> selector(frame, frame->stackPush(createString(BEER_WIDEN("$Class::createInstance()")))); // TODO

				StackRef<Method> method(frame, frame->stackPush());
				Class::findMethod(this, klass, selector, method);

				if(method.isNull())
				{
					throw MethodNotFoundException(*klass, ((Thread*)this)->getClass(klass), *selector); // TODO
				}

				StackRef<Object> ret(frame, frame->stackPush(NULL));
				frame->stackPush(*klass);

				StackFrame* nextFrame = openStackFrame();
				method->call(this); // pops copied mainClass
				closeStackFrame();

				instance = ret;

				frame->stackMoveTop(-3);
			}

			// call constuctor
			{
				StackRef<String> selector(frame, frame->stackPush());

				StackRef<String> tmpString1(frame, frame->stackPush());
				((Thread*)this)->createString(tmpString1, BEER_WIDEN("::")); // why the cast??, TODO

				StackRef<String> tmpString2(frame, frame->stackPush());
				Class::getName(this, klass, tmpString2);

				String::operatorAddString(this, tmpString2, tmpString1, selector); // Main::
				String::operatorAddString(this, selector, tmpString2, tmpString1); // Main::Main

				((Thread*)this)->createString(tmpString2, BEER_WIDEN("()")); // why the cast??, TODO

				String::operatorAddString(this, tmpString1, tmpString2, selector); // Main::Main()
				
				frame->stackMoveTop(-2); // pop tmpString1, tmpString2s

				StackRef<Method> method(frame, frame->stackPush());
				Class::findMethod(this, klass, selector, method);

				if(!method.isNull())
				{
					// ugly � create another trampoline
					TrampolineThread* thread = new TrampolineThread(this, mGC);
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

				StackRef<String> selector(frame, frame->stackPush(createString(BEER_WIDEN("Task::schedule()")))); // TODO

				StackRef<Method> method(frame, frame->stackPush());
				Class::findMethod(this, klass, selector, method);

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

		frame->stackMoveTop(-2); // pop class, substituable
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