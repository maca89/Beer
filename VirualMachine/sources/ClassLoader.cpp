#include "stdafx.h"
#include "ClassLoader.h"
#include "VirtualMachine.h"
#include "Class.h"
#include "Method.h"
#include "Param.h"
#include "Debugger.h"

using namespace Beer;


ClassLoader::ClassLoader()
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
	frame->stackPop(); // pop klass

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

	ret = thread->getPermanentHeap()->alloc<Class>(
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
			thread->getVM()->getMetaClass()
		));

		Object::setType(thread, ret, metaClass);
		frame->stackPop(); // pop metaClass
	}

	ret->mParentsCount = parents;
	ret->mPropertiesCount = properties;
	ret->mMethodsCount = methods;
	ret->mParentNext = ret->mMethodNext = ret->mPropertyNext = 0;
	ret->mTraverser = &Class::DefaultInstanceTraverser;

	// TODO: where??
	thread->getVM()->addClass(thread, *ret);

	ClassInitializer* initializer = getClassInitializer(classname->c_str());
	if(initializer) initializer->initClass(thread, this, ret);
}

void ClassLoader::createMethod(Thread* thread, StackRef<Method> ret, Cb fn, uint16 returns, uint16 params)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	ret = thread->getPermanentHeap()->alloc<Method>(
		Method::METHOD_CHILDREN_COUNT + returns + params
	);

	// set class
	{
		StackRef<Class> methodClass(frame, frame->stackPush(
			thread->getVM()->getMethodClass()
		));

		Object::setType(thread, ret, methodClass);
		frame->stackPop(); // pop methodClass
	}

	// init flags, TODO: is it really needed?
	ret->setFlags(0);
	ret->setMaxStack(20); // default value, TODO: get rid of
	ret->setBytecode(NULL);
	ret->setFunction(fn);
	ret->setTimeSpent(0);
	ret->setReturnsCount(returns);
	ret->setParamsCount(params);
}

void ClassLoader::addMethod(Thread* thread, StackRef<Class> klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Method> method(frame, frame->stackPush());
	createMethod(thread, method, fn, returns, params);

	// set name
	{
		StackRef<String> nameOnStack(frame, frame->stackPush());
		thread->createString(nameOnStack, name); // TODO: constant

		Method::setName(thread, method, nameOnStack);
		frame->stackPop(); // pop nameOnStack
	}

	addMethod(thread, klass, method, selector);
	frame->stackPop(); // pop method
}

void ClassLoader::addMethod(Thread* thread, StackRef<Class> klass, StackRef<Method> method, const char_t* selector)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();
	
	StackRef<Pair> pair(frame, frame->stackPush());

	// create pair
	{
		StackRef<String> selectorOnStack(frame, frame->stackPush());
		thread->createString(selectorOnStack, selector); // TODO

		thread->createPair(selectorOnStack, method, pair);
		frame->stackPop(); // pop selectorOnStack
	}

	Class::addMethod(thread, klass, pair);
	frame->stackPop(); // pop pair
}

void ClassLoader::createParam(Thread* thread, StackRef<Param> ret)
{
	ret = thread->getPermanentHeap()->alloc<Param>(
		Param::PARAM_CHILDREN_COUNT
	);
}

void ClassLoader::createProperty(Thread* thread, StackRef<Property> ret)
{
	ret = thread->getPermanentHeap()->alloc<Property>(
		Property::PROPERTY_CHILDREN_COUNT
	);
}