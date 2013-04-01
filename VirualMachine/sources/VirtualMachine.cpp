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
#include "InitVMTask.h"
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

void VirtualMachine::loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile)
{
	this->Thread::loadClassFile(loader, classFile); // TODO: Task
}

void VirtualMachine::run()
{
	//this->Thread::run();
	work();
}

/*void VirtualMachine::wait()
{
	this->Thread::wait();
}*/

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

	mScheduler.init(this, mGC);

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
	mClassLoader->addClassInitializer(BEER_WIDEN("InitVMTask"), new InitVMTaskInitializer);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// init objects and their classes
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		Frame* frame = getFrame();
		BEER_STACK_CHECK();

		InitVMTask* task = mHeap->alloc<InitVMTask>(InitVMTask::INITVMTASK_CHILDREN_COUNT);
		//task->setType(findClass(BEER_WIDEN("InitVMTask"))); // TODO

		// TODO: schedule

		// temporary solution
		{
			Frame* frame = getFrame();
			BEER_STACK_CHECK();
	
			StackRef<InitVMTask> taskOnStack(frame, frame->stackPush(task));
			InitVMTask::work(this, taskOnStack);

			frame->stackPop(); // pop taskOnStack
		}
	}

	// preloading of some classes
	mFloatClass = findClass(BEER_WIDEN("Float"));
	mBooleanClass = findClass(BEER_WIDEN("Boolean"));
	mIntegerClass = findClass(BEER_WIDEN("Integer"));
	mArrayClass = findClass(BEER_WIDEN("Array"));
	//mTaskClass = findClass(BEER_WIDEN("Task"));
	mPoolClass = findClass(BEER_WIDEN("Pool"));

	// !!! the order is important !!!
	////////////////////////////////////////////////////////////////// xxxxxxx0 // Object
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
		CreateAllEntryPointsTask::init(this, taskOnStack, taskOnStack);
		//CreateAllEntryPointsTask::work(this, taskOnStack);

		getScheduler()->schedule(this, taskOnStack.staticCast<Task>());

		frame->stackPop(); // pop taskOnStack
	}

	getScheduler()->resumeAll();
	//getScheduler()->

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

Class* VirtualMachine::findClass(string name)
{
	ClassReflectionTable::iterator it = mClasses.find(name); // TODO
	if(it != mClasses.end()) return mClasses.find(name)->second; // TODO

	{
		Frame* frame = getFrame();
		BEER_STACK_CHECK();

		StackRef<String> nameOnStack(frame, frame->stackPush());
		((Thread*)this)->createString(nameOnStack, name);

		mClassLoader->loadClass(this, nameOnStack);
		frame->stackPop(); // pop nameOnStack
	}

	// TODO
	it = mClasses.find(name); // TODO
	if(it != mClasses.end()) return mClasses.find(name)->second; // TODO

	throw ClassNotFoundException(name); // TODO
}

Class* VirtualMachine::findClass(StackRef<String> name)
{
	return findClass(name->c_str());
}