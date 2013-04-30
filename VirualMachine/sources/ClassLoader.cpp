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

Class* ClassLoader::createClass(Thread* thread, String* classname, uint32 staticSize, uint16 parents, uint16 properties, uint16 virtualMethods, uint16 interfaceMethods)
{
	// TODO
	//mVM->getMetaClass()->invoke(BEER_WIDEN("Class::createInstance"), );

	Class* klass = thread->getPermanentHeap()->alloc<Class>(
		staticSize, 
		Object::OBJECT_CHILDREN_COUNT + (parents - 1) + virtualMethods + interfaceMethods + properties
	);
	
	new(klass) Class(parents - 1, properties, virtualMethods, interfaceMethods);

	klass->setType(thread->getVM()->getMetaClass());
	klass->setName(classname);

	// TODO: where??
	thread->getVM()->addClass(thread, klass);

	ClassInitializer* initializer = getClassInitializer(classname->c_str());
	if(initializer)
	{
		initializer->initClass(thread, this, klass);
	}

	return klass;
}

Method* ClassLoader::createMethod(Thread* thread, String* name, Cb fn, uint16 returns, uint16 params)
{
	Method* method = thread->getPermanentHeap()->alloc<Method>(
		Method::METHOD_CHILDREN_COUNT + returns + params
	);

	new(method) Method(); // call ctor
	method->setFlags(0);
	method->setMaxStack(20); // default value, TODO: get rid of
	method->setBytecode(NULL);
	method->setFunction(fn);
	method->setTimeSpent(0);
	method->setReturnsCount(returns);
	method->setParamsCount(params);
	method->setType(thread->getVM()->getMethodClass());
	method->setName(name);
	
	return method;
}

Method* ClassLoader::addVirtualMethod(Thread* thread, Class* klass, Method* method, const char_t* cselector)
{
	method->setSelector(thread->createConstantString(cselector));
	klass->addVirtualMethod(method);
	return method;
}

Method* ClassLoader::addOverrideMethod(Thread* thread, Class* klass, Method* method, const char_t* cselector)
{
	uint32 index = 0;
	String* selector = thread->createConstantString(cselector);
	
	if(klass->findVirtualMethodIndex(selector, index))
	{
		method->setSelector(selector);
		klass->setMethod(index, method);
		//klass->addOverrideMethod(parent, method);
	}
	else
	{
		//klass->findVirtualMethodIndex(selector, index);
		throw Exception(BEER_WIDEN("Unable to override such method"));
	}

	return method;
}

Method* ClassLoader::addInterfaceMethod(Thread* thread, Class* klass, Class* interf, Method* method, const char_t* cselector)
{
	uint32 index = 0;
	String* selector = thread->createConstantString(cselector);
	
	if(klass->findInterfaceMethodIndex(interf, selector, index))
	{
		method->setSelector(selector);
		klass->setMethod(index, method);
		//klass->addOverrideMethod(parent, method);
	}
	else
	{
		throw Exception(BEER_WIDEN("Unable to override such method"));
	}

	return method;
}

Method* ClassLoader::addVirtualMethod(Thread* thread, Class* klass, const char_t* cname, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	Method* method = createMethod(thread, thread->createConstantString(cname), fn, returns, params);
	return addVirtualMethod(thread, klass, method, selector);
}

Method* ClassLoader::addOverrideMethod(Thread* thread, Class* klass, const char_t* cname, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	Method* method = createMethod(thread, thread->createConstantString(cname), fn, returns, params);
	return addOverrideMethod(thread, klass, method, selector);
}

Method* ClassLoader::addInterfaceMethod(Thread* thread, Class* klass, Class* interf, const char_t* cname, const char_t* selector, Cb fn, uint16 returns, uint16 params)
{
	Method* method = createMethod(thread, thread->createConstantString(cname), fn, returns, params);
	return addInterfaceMethod(thread, klass, interf, method, selector);
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