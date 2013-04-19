#include "stdafx.h"
#include "InitVMTask.h"
#include "Thread.h"
#include "Method.h"
#include "Class.h"
#include "MetaClass.h"
#include "ClassLoader.h"

using namespace Beer;


void BEER_CALL InitVMTask::init(Thread* thread, StackRef<InitVMTask> receiver, StackRef<InitVMTask> ret)
{
	ret = receiver;
}

void BEER_CALL InitVMTask::work(Thread* thread, StackRef<InitVMTask> receiver)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	ClassLoader* classLoader = thread->getVM()->getClassLoader();

	// create MetaClass
	String* metaClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (9 + 1)), // 9 for "MetaClass", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);

	metaClassName->size(9); // 9 for "MetaClass"
	metaClassName->copyData(BEER_WIDEN("MetaClass"));

	MetaClass* metaClass = thread->getPermanentHeap()->alloc<MetaClass>(
		Class::CLASS_CHILDREN_COUNT + 1 + 2 + Object::OBJECT_METHODS_COUNT // 1 parent, 2 methods
	);
	
	metaClass->mFlags = 0; // deprecated
	metaClass->mParentsCount = 1;
	metaClass->mPropertiesCount = 0;
	metaClass->mVirtualMethodsCount = 2 + Object::OBJECT_METHODS_COUNT;
	metaClass->mInterfaceMethodsCount = 0;
	metaClass->mParentNext = metaClass->mVirtualMethodNext = metaClass->mInterfaceMethodNext = metaClass->mPropertyNext = 0;
	metaClass->mTraverser = &MetaClass::DefaultClassTraverser;
	metaClass->mInstanceStaticSize = sizeof(Class);
	metaClass->setName(metaClassName); // set name
	thread->getVM()->setMetaClass(metaClass);


	// create Method class
	String* methodClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Method", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	methodClassName->size(6); // 6 for "Method"
	methodClassName->copyData(BEER_WIDEN("Method"));

	Class* methodClass = classLoader->createClass<Class>(thread, methodClassName, 1, 0, Object::OBJECT_METHODS_COUNT + 6); // extends Object, has 6 methods
	thread->getVM()->setMethodClass(methodClass);


	// create Frame class
	String* frameClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (5 + 1)), // 5 for "Frame", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	frameClassName->size(5); // 5 for "Frame"
	frameClassName->copyData(BEER_WIDEN("Frame"));

	Class* frameClass = classLoader->createClass<Class>(thread, frameClassName, 1, 0, Object::OBJECT_METHODS_COUNT); // extends Object, has 0 methods
	thread->getVM()->setFrameClass(frameClass);


	// create Object class
	String* objectClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Object", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	objectClassName->size(6); // 6 for "Object"
	objectClassName->copyData(BEER_WIDEN("Object"));

	Class* objectClass = classLoader->createClass<Class>(thread, objectClassName, 1, 0, Object::OBJECT_METHODS_COUNT); // extends Object, has 6 methods
	thread->getVM()->setObjectClass(objectClass);


	// create String class
	String* stringClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "String", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	stringClassName->size(6); // 6 for "String"
	stringClassName->copyData(BEER_WIDEN("String"));

	Class* stringClass = classLoader->createClass<Class>(thread, stringClassName, 1, 0, 17 + Object::OBJECT_METHODS_COUNT);
	thread->getVM()->setStringClass(stringClass);


	// fix references
	metaClassName->setType(stringClass);
	objectClassName->setType(stringClass);
	stringClassName->setType(stringClass);
	methodClassName->setType(stringClass);
	frameClassName->setType(stringClass);

	// default Object methods
	classLoader->addVirtualMethod(thread, objectClass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Class::createInstance, 1, 0);
	classLoader->addVirtualMethod(thread, objectClass, BEER_WIDEN("Object"), BEER_WIDEN("Object::Object()"), &Object::init, 1, 0);
	classLoader->addVirtualMethod(thread, objectClass, BEER_WIDEN("String"), BEER_WIDEN("Object::String()"), &Object::operatorString, 1, 0);
	// TODO: getType

	// fix references
	metaClass->setSuperClass(objectClass);
	objectClass->setSuperClass(objectClass);
	methodClass->setSuperClass(objectClass);
	frameClass->setSuperClass(objectClass);

	// must be *after* super class set
	methodClass->setTraverser(&Method::MethodTraverser);
	frameClass->setTraverser(&Frame::FrameTraverser);

	// init string class
	StringClassInitializer* stringInit = new StringClassInitializer();
	stringInit->initClass(thread, classLoader, stringClass);
	delete stringInit;

	// fix metaclass methods
	classLoader->addVirtualMethod(thread, metaClass, BEER_WIDEN("MetaClass"), BEER_WIDEN("MetaClass::MetaClass()"), &MetaClass::init, 1, 0);
	//mClassLoader->addMethod(*(metaClass.staticCast<Class>()), BEER_WIDEN("findMethod"), BEER_WIDEN("Class::findMethod(String)"), &MetaClass::findMethod, 1, 1);

	// TODO: fix Method methods
}

Class* InitVMTaskInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, InitVMTask::INITVMTASK_CHILDREN_COUNT, InitVMTask::INITVMTASK_METHODS_COUNT);
}

void InitVMTaskInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->setSuperClass(thread->getVM()->findClass(BEER_WIDEN("Task")));
	klass->markSealed();
	
	loader->addVirtualMethod(thread, klass, BEER_WIDEN("InitVMTask"), BEER_WIDEN("InitVMTask::InitVMTask()"), &InitVMTask::init, 1, 0);
	loader->addOverrideMethod(thread, klass, BEER_WIDEN("run"), BEER_WIDEN("Task::run()"), &InitVMTask::work, 0, 0);
}