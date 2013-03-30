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

	Class* klass = classInit->createClass(thread, this, *name);

	mBeingLoaded.pop_back();

	delete it->second; // ugly
	mClassInitializers.erase(it);
}

bool ClassLoader::canLoadClass(string name)
{
	return hasClassInitializer(name);
}

Class* ClassLoader::createClass(Thread* thread, String* classname, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods)
{
	// TODO
	//mVM->getMetaClass()->invoke(BEER_WIDEN("Class::createInstance"), );

	Class* klass = thread->getPermanentHeap()->alloc<Class>(
		staticSize, 
		Object::OBJECT_CHILDREN_COUNT + 1 + parents + methods + properties // +1 for name
	);
	
	klass->mFlags = 0;
	klass->mParentsCount = parents;
	klass->mPropertiesCount = properties;
	klass->mMethodsCount = methods;
	klass->mParentNext = klass->mMethodNext = klass->mPropertyNext = 0;
	klass->mTraverser = &Class::DefaultInstanceTraverser;

	klass->setName(classname);
	klass->setType(thread->getVM()->getMetaClass());

	// TODO: where??
	thread->getVM()->addClass(thread, klass);

	ClassInitializer* initializer = getClassInitializer(classname->c_str());
	if(initializer)
	{
		initializer->initClass(thread, this, klass);
	}

	return klass;
}

Method* ClassLoader::createMethod(Thread* thread, Cb fn, uint16 returns, uint16 params)
{
	Method* method = thread->getPermanentHeap()->alloc<Method>(
		Method::METHOD_CHILDREN_COUNT + returns + params
	);

	// init flags, TODO: is it really needed?
	method->setFlags(0);
	method->setMaxStack(20); // default value, TODO: get rid of
	method->setBytecode(NULL);
	method->setFunction(fn);
	method->setTimeSpent(0);
	method->setReturnsCount(returns);
	method->setParamsCount(params);
	method->setType(thread->getVM()->getMethodClass());
	
	return method;
}

Method* ClassLoader::addMethod(Thread* thread, Class* klass, const char_t* name, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	Method* method = createMethod(thread, fn, returns, params);

	// set name
	{
		Frame* frame = thread->getFrame();
		BEER_STACK_CHECK();

		StackRef<String> nameOnStack(frame, frame->stackPush());
		thread->createString(nameOnStack, name); // TODO: constant

		method->setName(*nameOnStack);
		frame->stackPop(); // pop nameOnStack
	}

	addMethod(thread, klass, method, selector);
	return method;
}

Method* ClassLoader::addMethod(Thread* thread, Class* klass, Method* method, const char_t* selector)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();
	
	StackRef<Pair> pair(frame, frame->stackPush());

	// create pair
	{
		StackRef<String> selectorOnStack(frame, frame->stackPush());
		thread->createString(selectorOnStack, selector); // TODO: constant

		StackRef<Method> methodOnStack(frame, frame->stackPush(
			method
		));

		thread->createPair(selectorOnStack, methodOnStack, pair);
		frame->stackPop(); // pop methodOnStack
		frame->stackPop(); // pop selectorOnStack
	}

	klass->addMethod(*pair);
	frame->stackPop(); // pop pair

	return method;
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