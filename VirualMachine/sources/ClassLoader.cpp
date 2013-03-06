#include "stdafx.h"
#include "ClassLoader.h"
#include "VirtualMachine.h"
#include "Heap.h"
#include "Class.h"
#include "Method.h"
#include "Param.h"

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

	//mVM->getMetaClass()->invoke(BEER_WIDEN("Class::createInstance"), );

	Class* klass = mClassHeap->alloc<Class>(
		staticSize, 
		Object::OBJECT_CHILDREN_COUNT + 1 + parents + methods + properties // +1 for name
	);
	
	klass->mFlags = 0;
	klass->setName(classname);
	klass->setClass(mVM->getMetaClass());
	klass->mParentsCount = parents;
	klass->mPropertiesCount = properties;
	klass->mMethodsCount = methods;
	//klass->mDefaultChildrenCount = Object::OBJECT_CHILDREN_COUNT + properties;

	// TODO: where??
	mVM->addClass(klass);

	return klass;
}

Method* ClassLoader::createMethod(uint16 returns, uint16 params)
{
	Method* method = mClassHeap->alloc<Method>(
		Method::METHOD_CHILDREN_COUNT + returns + params + 10
	);

	new(method) Method(); // init __vtable, Warning: may cause troubles in DEBUG, ctor sets debug values

	// init flags, TODO: is it really needed?
	method->mFlags = 0;

	method->setMaxStack(20); // default value, TODO: get rid of
	
	// TODO: garbaged
	method->mReturnsCount = returns;
	method->mParamsCount = params;

	return method;
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


