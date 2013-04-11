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
	MetaClass* metaClass = thread->getPermanentHeap()->alloc<MetaClass>(
		Class::CLASS_CHILDREN_COUNT + 1 + 2 + Object::OBJECT_METHODS_COUNT // 1 parent, 2 methods
	);

	metaClass->mFlags = 0; // deprecated
	metaClass->mParentsCount = 1;
	metaClass->mPropertiesCount = 0;
	metaClass->mMethodsCount = 2 + Object::OBJECT_METHODS_COUNT;
	metaClass->mParentNext = metaClass->mMethodNext = metaClass->mPropertyNext = 0;
	metaClass->mTraverser = &MetaClass::DefaultClassTraverser;
	metaClass->mInstanceStaticSize = sizeof(Class);
	thread->getVM()->setMetaClass(metaClass);

	// create MetaClass name
	String* metaClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (9 + 1)), // 9 for "MetaClass", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);

	metaClassName->size(9); // 9 for "MetaClass"
	metaClassName->copyData(BEER_WIDEN("MetaClass"));
	metaClass->setName(metaClassName); // set name

	// create Object class name
	String* objectClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Object", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	objectClassName->size(6); // 6 for "Object"
	objectClassName->copyData(BEER_WIDEN("Object"));

	// create Method class name
	String* methodClassName = thread->getPermanentHeap()->alloc<String>(
		static_cast<uint32>(sizeof(String) + sizeof(String::CharacterData) * (6 + 1)), // 6 for "Method", 1 for "\0"
		Object::OBJECT_CHILDREN_COUNT + 0 // TODO: size
	);
	methodClassName->size(6); // 6 for "Method"
	methodClassName->copyData(BEER_WIDEN("Method"));


	// create Method class
	Class* methodClass = classLoader->createClass<Class>(thread, methodClassName, 1, 0, Object::OBJECT_METHODS_COUNT + 6); // extends Object, has 6 methods
	methodClass->setTraverser(&Method::MethodTraverser);
	thread->getVM()->setMethodClass(methodClass);

	// create Object class
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

	// init string class
	StringClassInitializer* stringInit = new StringClassInitializer();
	stringInit->initClass(thread, classLoader, stringClass);
	delete stringInit;

	// fix references
	metaClassName->setType(stringClass); // set class
	objectClassName->setType(stringClass); // set class
	stringClassName->setType(stringClass); // set class

	// fix metaclass methods
	classLoader->addMethod(thread, metaClass, BEER_WIDEN("MetaClass"), BEER_WIDEN("MetaClass::MetaClass()"), &MetaClass::init, 1, 0);
	//mClassLoader->addMethod(*(metaClass.staticCast<Class>()), BEER_WIDEN("findMethod"), BEER_WIDEN("Class::findMethod(String)"), &MetaClass::findMethod, 1, 1);

	// default Object methods
	classLoader->addMethod(thread, objectClass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &Class::createInstance, 1, 0);
	classLoader->addMethod(thread, objectClass, BEER_WIDEN("Object"), BEER_WIDEN("Object::Object()"), &Object::init, 1, 0);
	classLoader->addMethod(thread, objectClass, BEER_WIDEN("String"), BEER_WIDEN("Object::String()"), &Object::operatorString, 1, 0);
	// TODO: getType

	// fix references
	metaClass->addParent(objectClass);
	objectClass->addParent(objectClass);
	methodClass->addParent(objectClass);

	// TODO: fix Method methods
}

Class* InitVMTaskInitializer::createClass(Thread* thread, ClassLoader* loader, String* name)
{
	return loader->createClass<Class>(thread, name, 1, InitVMTask::INITVMTASK_CHILDREN_COUNT, InitVMTask::INITVMTASK_METHODS_COUNT);
}

void InitVMTaskInitializer::initClass(Thread* thread, ClassLoader* loader, Class* klass)
{
	klass->addParent(thread->getVM()->findClass(BEER_WIDEN("Task")));
	
	loader->addMethod(thread, klass, BEER_WIDEN("InitVMTask"), BEER_WIDEN("InitVMTask::InitVMTask()"), &InitVMTask::init, 1, 0);
	loader->addMethod(thread, klass, BEER_WIDEN("work"), BEER_WIDEN("Task::work()"), &InitVMTask::work, 0, 0); // interface method, TODO: second selector
}