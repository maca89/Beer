#include "stdafx.h"
#include "ClassLoader.h"
#include "VirtualMachine.h"

#include "Class.h"
#include "MethodReflection.h"
#include "ParamReflection.h"

using namespace Beer;


ClassLoader::ClassLoader(VirtualMachine* vm, GarbageCollector* heap) : mVM(vm), mClassHeap(heap)
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

//////////////////// ---------- classes ---------- ////////////////////

/*uint32 ClassLoader::countClassDynamicSize(string name, uint16 parents, uint16 properties, uint16 methods)
{
	return (name.size() + 2) * sizeof(char_t*); // +1 for \0;
}*/

Class* ClassLoader::createClass(String* classname, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods)
{
	//uint32 dynamicSize = countClassDynamicSize(classname, parents, properties, methods);

	Class* klass = mClassHeap->alloc<Class>(
		staticSize/* + dynamicSize*/, 
		Object::OBJECT_CHILDREN_COUNT + 1 + parents + methods + properties // +1 for name
	);
	
	// init flags
	klass->mFlags = 0;

	// init name
	klass->setName(classname); // TODO
	/*klass->mNameLength = classname.size();
	klass->mName = reinterpret_cast<char_t*>(reinterpret_cast<byte*>(klass) + staticSize);
	strcpy(klass->mName, classname.size() + 1, classname.c_str());*/

	klass->mParentsCount = parents;
	klass->mPropertiesCount = properties;
	klass->mMethodsCount = methods;
	klass->mDefaultChildrenCount = Object::OBJECT_CHILDREN_COUNT + properties;

	// TODO: where??
	mVM->addClass(klass);

	return klass;
}

//////////////////// ---------- methods ---------- ////////////////////

MethodReflection* ClassLoader::createMethod(uint32 staticSize, uint16 returns, uint16 params)
{
	MethodReflection* method = mClassHeap->alloc<MethodReflection>(
		staticSize,
		MethodReflection::METHOD_CHILDREN_COUNT + returns + params + 10
	);

	// init flags, TODO: is it really needed?
	method->mFlags = 0;

	method->setMaxStack(100); // default value, TODO: get rid of
	
	// TODO: garbaged
	method->mReturnsCount = returns;
	method->mParamsCount = params;

	return method;
}

//////////////////// ---------- params ---------- ////////////////////

ParamReflection* ClassLoader::createParam(uint32 staticSize)
{
	ParamReflection* param = mClassHeap->alloc<ParamReflection>(
		staticSize,
		ParamReflection::PARAM_CHILDREN_COUNT
	);

	return param;
}

//////////////////// ---------- properties ---------- ////////////////////

PropertyReflection* ClassLoader::createProperty(uint32 staticSize)
{
	PropertyReflection* prop = mClassHeap->alloc<PropertyReflection>(
		staticSize,
		PropertyReflection::PROPERTY_CHILDREN_COUNT
	);

	return prop;
}


