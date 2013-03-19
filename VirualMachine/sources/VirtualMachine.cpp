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
			mMetaClass = getPermanentHeap()->alloc<MetaClass>(
				Class::CLASS_CHILDREN_COUNT + 1 + 2 + Object::OBJECT_METHODS_COUNT // 1 parent, 2 methods
		)));

		mMetaClass = *metaClass; // ugly, TODO
		metaClass->mFlags = 0; // deprecated
		metaClass->mParentsCount = 1;
		metaClass->mPropertiesCount = 0;
		metaClass->mMethodsCount = 2 + Object::OBJECT_METHODS_COUNT;
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

		StackRef<Class> objectClass(frame, frame->stackPush());

		// create Object class
		{
			mClassLoader->createClass<Class>(this, objectClassName, objectClass, 1, 0, Object::OBJECT_METHODS_COUNT); // extends Object, has 6 methods

			// fix references
			Class::addParent(this, metaClass.staticCast<Class>(), objectClass);
			Class::addParent(this, objectClass, objectClass);

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
			Object::setChild(static_cast<Thread*>(this), objectClassName, stringClass, childIdClass); // set class
			Object::setChild(static_cast<Thread*>(this), stringClassName, stringClass, childIdClass); // set class

			frame->stackMoveTop(-2); // pop stringClassName & stringClass
		}

		// fix metaclass methods
		mClassLoader->addMethod(this, metaClass.staticCast<Class>(), BEER_WIDEN("MetaClass"), BEER_WIDEN("MetaClass::MetaClass()"), &MetaClass::init, 1, 0);
		//mClassLoader->addMethod(*(metaClass.staticCast<Class>()), BEER_WIDEN("findMethod"), BEER_WIDEN("Class::findMethod(String)"), &MetaClass::findMethod, 1, 1);

		// default Object methods
		mClassLoader->addMethod(this, objectClass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Class::createInstance, 1, 0);
		mClassLoader->addMethod(this, objectClass, BEER_WIDEN("Object"), BEER_WIDEN("Object::Object()"), &Object::init, 1, 0);
		mClassLoader->addMethod(this, objectClass, BEER_WIDEN("String"), BEER_WIDEN("Object::String()"), &Object::operatorString, 1, 0);


		frame->stackMoveTop(-6); // clean
		//DBG_ASSERT(frame->stackSize() == 2, BEER_WIDEN("Stack was not properly cleaned"));
		//closeFrame();
	}

	// preloading of some classes
	mFloatClass = findClass(BEER_WIDEN("Float"));
	mBooleanClass = findClass(BEER_WIDEN("Boolean"));
	mIntegerClass = findClass(BEER_WIDEN("Integer"));
	mArrayClass = findClass(BEER_WIDEN("Array"));
	//mTaskClass = findClass(BEER_WIDEN("Task"));

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
	Frame* frame = getFrame();
	StackRef<Class> entryPointClass(frame, frame->stackPush(findClass(BEER_WIDEN("EntryPoint")))); // TODO

	if(entryPointClass.isNull())
	{
		throw ClassNotFoundException(BEER_WIDEN("Class EntryPoint not found"));
	}

	for(ClassReflectionTable::iterator it = mClasses.begin(); it != mClasses.end(); it++)
	{
		StackRef<Class> klass(frame, frame->stackPush(it->second)); // TODO
		bool substituable = false;

		// print classname
#ifdef BEER_FOLDING_BLOCK
		if(false){
			StackRef<String> klassName(frame, frame->stackPush());
			Class::getName(this, klass, klassName);
			cout << klassName->c_str() << "\n";

			if(string(klassName->c_str()).compare(BEER_WIDEN("Main")) == 0)
			{
				int a = 0;
			}

			frame->stackMoveTop(-1); // pop klassName
		}
#endif // BEER_FOLDING_BLOCK

		// fetch substituable
		{
			StackRef<Boolean> tmp(frame, frame->stackPush());
			Class::substituable(this, klass, entryPointClass, tmp);
			substituable = tmp->getData();
			frame->stackMoveTop(-1); // pop tmp
		}

		if(klass.get() != entryPointClass.get() && substituable)  // TODO
		{
			StackRef<Object> instance(frame, frame->stackPush(NULL));

			// create new instance
#ifdef BEER_FOLDING_BLOCK
			{
				StackRef<Method> method(frame, frame->stackPush());
				
				// fetch method
				{
#ifdef BEER_FOLDING_BLOCK
					StackRef<String> selector(frame, frame->stackPush());
					((Thread*)this)->createString(selector, BEER_WIDEN("$Class::createInstance()")); // TODO

					Class::findMethod(this, klass, selector, method);

					if(method.isNull())
					{
						throw MethodNotFoundException(*klass, ((Thread*)this)->getType(klass), *selector); // TODO
					}

					frame->stackMoveTop(-1); // pop selector
#endif // BEER_FOLDING_BLOCK
				}

				StackRef<Object> ret(frame, frame->stackPush());
				frame->stackPush(*klass);
				frame->stackPush(*method);

				openFrame();
				method->call(this); // pops copied klass, copied method

				instance = *ret;
				frame->stackMoveTop(-2); // pop method, ret
			}
#endif // BEER_FOLDING_BLOCK


			// call constuctor
#ifdef BEER_FOLDING_BLOCK
			if(true){
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
					// ugly – create another trampoline
					TrampolineThread* thread = new TrampolineThread(this, mGC);
					getThreads().insert(thread);

					thread->getFrame()->stackMoveTop(1); // for return
					thread->getFrame()->stackPush(*instance); // push receiver
					thread->getFrame()->stackPush(*method); // push method
					thread->openFrame();

					thread->run();
					thread->wait();

					//instance = thread->getFrame()->stackTop(); // fix main // TODO
				}

				frame->stackMoveTop(-2); // pop method & selector
			}
#endif // BEER_FOLDING_BLOCK


			// Task::run()
			{
#ifdef BEER_FOLDING_BLOCK
				StackRef<Method> method(frame, frame->stackPush());
				//getDebugger()->printFrameStack(frame);
				
				// fetch method
				{
#ifdef BEER_FOLDING_BLOCK
					StackRef<String> selector(frame, frame->stackPush()); // TODO
					((Thread*)this)->createString(selector, BEER_WIDEN("Task::schedule()")); // why the cast?!, TODO

					Class::findMethod(this, klass, selector, method);

					if(method.isNull())
					{
						throw MethodNotFoundException(*instance, *klass, *selector); // TODO
					}
					frame->stackMoveTop(-1); // pop selector
#endif // BEER_FOLDING_BLOCK
				}

				//getDebugger()->printFrameStack(frame);

				openFrame();
				method->call(this);
#endif // BEER_FOLDING_BLOCK
			}
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
	
	DBG_ASSERT(frame->stackSize() == 2, BEER_WIDEN("Stack was not properly cleaned"));
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

	frame->stackMoveTop(-1); // pop string
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
	frame->stackMoveTop(-1);
	return p;
}

Class* VirtualMachine::findClass(string name)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<String> nameOnStack(frame, frame->stackPush());
	((Thread*)this)->createString(nameOnStack, name);
	
	Class* c = findClass(nameOnStack);
	frame->stackMoveTop(-1); // pop nameOnStack
	return c;
}