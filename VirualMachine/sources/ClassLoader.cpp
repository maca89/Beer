#include "stdafx.h"
#include "ClassLoader.h"
#include "VirtualMachine.h"
#include "Heap.h"
#include "Class.h"
#include "Method.h"
#include "Param.h"
#include "Debugger.h"

using namespace Beer;


ClassLoader::ClassLoader(VirtualMachine* vm, Heap* heap) : mVM(vm), mClassHeap(heap)
{
}

ClassLoader::~ClassLoader()
{
}

void ClassLoader::addClassInitializer(string classname, ClassInitializer* initializer)
{
	mClassInitializers[classname] = initializer; // memory leak!, TODO
}

ClassInitializer* ClassLoader::getClassInitializer(string classname)
{
	ClassInitializerMap::iterator it = mClassInitializers.find(classname);
	if(it != mClassInitializers.end())
	{
		return it->second;
	}
	return NULL;
}

void ClassLoader::removeClassInitializer(string classname)
{
	ClassInitializerMap::iterator it = mClassInitializers.find(classname);
	if(it != mClassInitializers.end())
	{
		delete it->second; // ugly
		mClassInitializers.erase(it);
	}
}

bool ClassLoader::hasClassInitializer(string name) const
{
	return mClassInitializers.find(name) != mClassInitializers.end();
}

void ClassLoader::loadClass(String* name)
{
	for(StringList::iterator it = mBeingLoaded.begin(); it != mBeingLoaded.end(); it++)
	{
		if(strcmp(it->c_str(), name->c_str()) == 0) // TODO
		{
			throw CircularParentException(name->c_str()); // TODO
		}
	}

	ClassInitializerMap::iterator it = mClassInitializers.find(name->c_str()); // TODO
	if(it == mClassInitializers.end())
	{
		return; // false
	}

	mBeingLoaded.push_back(name->c_str()); // TODO
	ClassInitializer* classInit = it->second;
	Class* klass = classInit->createClass(mVM, this, name);
	//mVM->addClass(klass); // where to put this???
	mBeingLoaded.pop_back();

	delete it->second; // ugly
	mClassInitializers.erase(it);
}

bool ClassLoader::canLoadClass(string name)
{
	return hasClassInitializer(name);
}

Class* ClassLoader::createClass(String* classname, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods)
{
	// TODO
	//mVM->getMetaClass()->invoke(BEER_WIDEN("Class::createInstance"), );

	Thread* thread = mVM;
	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush(
		mClassHeap->alloc<Class>(
			staticSize, 
			Object::OBJECT_CHILDREN_COUNT + 1 + parents + methods + properties // +1 for name
	)));
	
	klass->mFlags = 0;
	
	// set name
	{
		StackRef<String> name(frame, frame->stackPush(classname)); // push name

		Class::setName(thread, klass, name);
		frame->stackMoveTop(-1); // pop name
	}

	// set class
	{
		StackRef<Class> metaClass(frame, frame->stackPush(
			mVM->getMetaClass()
		));

		Class::setClass(thread, klass, metaClass);
		frame->stackMoveTop(-1); // pop metaClass
	}

	klass->mParentsCount = parents;
	klass->mPropertiesCount = properties;
	klass->mMethodsCount = methods;
	klass->mParentNext = klass->mMethodNext = klass->mPropertyNext = 0;

	// TODO: where??
	mVM->addClass(*klass);

	Class* c = *klass;
	frame->stackMoveTop(-1); // pop class

	return c;
}

Method* ClassLoader::createMethod(Cb fn, uint16 returns, uint16 params)
{
	Method* method = mClassHeap->alloc<Method>(
		Method::METHOD_CHILDREN_COUNT + returns + params + 10
	);

	new(method) Method(); // init __vtable, Warning: may cause troubles in DEBUG, ctor sets debug values

	// init flags, TODO: is it really needed?
	method->mFlags = 0;

	method->setMaxStack(20); // default value, TODO: get rid of
	
	method->setFunction(fn);
	
	// TODO: garbaged
	method->mReturnsCount = returns;
	method->mParamsCount = params;

	return method;
}

void ClassLoader::addMethod(Class* klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	StackFrame* frame = mVM->getStackFrame();
	BEER_STACK_CHECK();
	
	StackRef<Class> klassOnStack(frame, frame->stackPush(klass));
	StackRef<Pair> pair(frame, frame->stackPush());

	// create pair
	{
		StackRef<Method> method(frame, frame->stackPush(
			createMethod(fn, returns, params)
		));

		StackRef<String> str(frame, frame->stackPush());
		((Thread*)mVM)->createString(str, name); // TODO: constant

		Method::setName(mVM, method, str);
		
		((Thread*)mVM)->createString(str, selector); // TODO: constant
		((Thread*)mVM)->createPair(str, method, pair); // pops str, method
	}

	Class::addMethod(mVM, klassOnStack, pair);
	frame->stackMoveTop(-2); // pop class, pair
}

void ClassLoader::addMethod(Class* klass, Method* method, const char_t* selector)
{
	Thread* thread = (Thread*)mVM; // TODO
	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();
	
	StackRef<Class> klassOnStack(frame, frame->stackPush(klass));
	StackRef<Pair> pair(frame, frame->stackPush());
	StackRef<Method> methodOnStack(frame, frame->stackPush(method));
	StackRef<String> selectorOnStack(frame, frame->stackPush());
		
	thread->createString(selectorOnStack, selector); // TODO
	thread->createPair(selectorOnStack, methodOnStack, pair); // pops selector, method
			
	Class::addMethod(thread, klassOnStack, pair);
	frame->stackMoveTop(-2); // pop class, pair
}

Param* ClassLoader::createParam()
{
	Param* param = mClassHeap->alloc<Param>(
		Param::PARAM_CHILDREN_COUNT
	);

	//new(param) T(); // init __vtable, Warning: may cause troubles in DEBUG, ctor sets debug values

	return param;
}

Property* ClassLoader::createProperty()
{
	Property* prop = mClassHeap->alloc<Property>(
		Property::PROPERTY_CHILDREN_COUNT
	);

	//new(prop) T(); // init __vtable, Warning: may cause troubles in DEBUG, ctor sets debug values

	return prop;
}

void ClassLoader::extendClass(StackRef<Class> klass, StackRef<Class> extending)
{
	Class::addParent(mVM, klass, extending);
}

void ClassLoader::extendClass(Class* klass, Class* extending)
{
	StackFrame* frame = mVM->getStackFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klassOnStack(frame, frame->stackPush(klass));
	StackRef<Class> extendingOnStack(frame, frame->stackPush(extending));

	extendClass(klassOnStack, extendingOnStack);

	frame->stackMoveTop(-2); // pop class & extending
}
