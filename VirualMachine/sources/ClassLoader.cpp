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

void ClassLoader::loadClass(Thread* thread, StackRef<String> name)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

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

	StackRef<Class> klass(frame, frame->stackPush());
	classInit->createClass(thread, this, name, klass);
	//mVM->addClass(klass); // where to put this???
	frame->stackMoveTop(-1); // pop klass

	mBeingLoaded.pop_back();

	delete it->second; // ugly
	mClassInitializers.erase(it);
}

bool ClassLoader::canLoadClass(string name)
{
	return hasClassInitializer(name);
}

void ClassLoader::createClass(Thread* thread, StackRef<String> classname, StackRef<Class> ret, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods)
{
	// TODO
	//mVM->getMetaClass()->invoke(BEER_WIDEN("Class::createInstance"), );

	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	ret = mClassHeap->alloc<Class>(
			staticSize, 
			Object::OBJECT_CHILDREN_COUNT + 1 + parents + methods + properties // +1 for name
	);
	
	ret->mFlags = 0;
	
	// set name
	{
		Class::setName(thread, ret, classname);
	}

	// set class
	{
		StackRef<Class> metaClass(frame, frame->stackPush(
			mVM->getMetaClass()
		));

		Class::setClass(thread, ret, metaClass);
		frame->stackMoveTop(-1); // pop metaClass
	}

	ret->mParentsCount = parents;
	ret->mPropertiesCount = properties;
	ret->mMethodsCount = methods;
	ret->mParentNext = ret->mMethodNext = ret->mPropertyNext = 0;

	// TODO: where??
	mVM->addClass(*ret);
}

void ClassLoader::createMethod(StackRef<Method> ret, Cb fn, uint16 returns, uint16 params)
{
	ret = mClassHeap->alloc<Method>(
		Method::METHOD_CHILDREN_COUNT + returns + params + 10
	);

	new(*ret) Method(); // init __vtable, Warning: may cause troubles in DEBUG, ctor sets debug values

	// init flags, TODO: is it really needed?
	ret->mFlags = 0;

	ret->setMaxStack(20); // default value, TODO: get rid of
	
	ret->setFunction(fn);
	
	// TODO: garbaged
	ret->mReturnsCount = returns;
	ret->mParamsCount = params;
}

void ClassLoader::addMethod(StackRef<Class> klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	Frame* frame = mVM->getFrame();
	BEER_STACK_CHECK();
	
	StackRef<Pair> pair(frame, frame->stackPush());

	// create pair
	{
		StackRef<Method> method(frame, frame->stackPush());
		createMethod(method, fn, returns, params);

		StackRef<String> str(frame, frame->stackPush());
		((Thread*)mVM)->createString(str, name); // TODO: constant

		Method::setName(mVM, method, str);
		
		((Thread*)mVM)->createString(str, selector); // TODO: constant
		((Thread*)mVM)->createPair(str, method, pair);
		frame->stackMoveTop(-2); // pop str, method
	}

	Class::addMethod(mVM, klass, pair);
	frame->stackMoveTop(-1); // pop pair
}

void ClassLoader::addMethod(StackRef<Class> klass, StackRef<Method> method, const char_t* selector)
{
	Thread* thread = (Thread*)mVM; // TODO
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();
	
	StackRef<Pair> pair(frame, frame->stackPush());

	// create pair
	{
		StackRef<String> selectorOnStack(frame, frame->stackPush());
		thread->createString(selectorOnStack, selector); // TODO

		thread->createPair(selectorOnStack, method, pair);
		frame->stackMoveTop(-1); // pop selectorOnStack
	}

	Class::addMethod(thread, klass, pair);
	frame->stackMoveTop(-1); // pop pair
}

void ClassLoader::createParam(StackRef<Param> ret)
{
	ret = mClassHeap->alloc<Param>(
		Param::PARAM_CHILDREN_COUNT
	);
}

void ClassLoader::createProperty(StackRef<Property> ret)
{
	ret = mClassHeap->alloc<Property>(
		Property::PROPERTY_CHILDREN_COUNT
	);
}

void ClassLoader::extendClass(StackRef<Class> klass, StackRef<Class> extending)
{
	Class::addParent(mVM, klass, extending);
}