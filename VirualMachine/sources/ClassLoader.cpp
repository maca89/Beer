#include "stdafx.h"
#include "ClassLoader.h"
#include "VirtualMachine.h"
#include "MarkAndSweepGC.h"

#include "ClassReflection.h"
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
	mClassInitializers[classname] = initializer; // memory leak!
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

void ClassLoader::loadClass(string name)
{
	//cout << "ClassLoader: loading " << name << std::endl;

	for(StringList::iterator it = mBeingLoaded.begin(); it != mBeingLoaded.end(); it++)
	{
		if(*it == name)
		{
			throw CircularParentException(name);
		}
	}

	ClassInitializerMap::iterator it = mClassInitializers.find(name);
	if(it == mClassInitializers.end())
	{
		return; // false
	}

	mBeingLoaded.push_back(name);
	ClassInitializer* classInit = it->second;
	ClassReflection* klass = classInit->createClass(mVM, this, name);
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

uint32 ClassLoader::countClassDynamicSize(string name, uint16 parents, uint16 properties, uint16 methods)
{
	return (name.size() + 1) * sizeof(char_t) // +1 for \0
		+ parents * sizeof(ClassReflection*)
		+ properties * sizeof(PropertyReflection*)
		+ methods * sizeof(MethodReflection*);
}

ClassReflection* ClassLoader::createClass(string classname, uint32 staticSize, uint16 childrenCount, uint16 parents, uint16 properties, uint16 methods)
{
	uint32 dynamicSize = countClassDynamicSize(classname, parents, properties, methods);

	ClassReflection* klass = mClassHeap->alloc<ClassReflection>(staticSize + dynamicSize, childrenCount);
	initClass(klass, classname, staticSize, parents, properties, methods);
	
	mVM->addClass(klass);
	
	ClassInitializer* initializer = getClassInitializer(classname);
	if(initializer) initializer->initClass(mVM, this, klass);

	return klass;
}

void ClassLoader::initClass(ClassReflection* klass, string name, uint32 staticSize, uint16 parents, uint16 properties, uint16 methods)
{
	byte* ptr = reinterpret_cast<byte*>(klass) + staticSize; // move ptr at the end of static-sized object part

	// init flags
	klass->mFlags = 0;

	// init name
	klass->mNameCount = name.size() + 1; // +1 for \0
	klass->mName = reinterpret_cast<char_t*>(ptr);
	memcpy(klass->mName, name.c_str(), klass->mNameCount * sizeof(char_t)); // c_str is terminated with \0

	ptr += klass->mNameCount * sizeof(char_t); // move ptr after string

	// init parents
	klass->mParentsCount = parents;
	klass->mParents = reinterpret_cast<ClassReflection**>(ptr);
	for(uint16 i = 0; i < klass->mParentsCount; i++)
	{
		klass->mParents[i] = NULL;
	}

	ptr += klass->mParentsCount * sizeof(ClassReflection*); // move ptr after parents

	// init properties
	klass->mPropertiesCount = properties;
	klass->mDefaultChildrenCount = properties;
	klass->mProperties = reinterpret_cast<PropertyReflection**>(ptr);
	for(uint16 i = 0; i < klass->mPropertiesCount; i++)
	{
		klass->mProperties[i] = NULL;
	}

	ptr += klass->mPropertiesCount * sizeof(PropertyReflection*); // move ptr after properties

	// init methods
	klass->mMethodsCount = methods;
	klass->mMethods = reinterpret_cast<MethodReflection**>(ptr);
	for(uint16 i = 0; i < klass->mPropertiesCount; i++)
	{
		klass->mProperties[i] = NULL;
	}
	
	ptr += klass->mMethodsCount * sizeof(MethodReflection*); // move ptr after methods
}

//////////////////// ---------- methods ---------- ////////////////////

uint32 ClassLoader::countMethodDynamicSize(string name, string selector, uint16 returns, uint16 params)
{
	return (name.size() + 1) * sizeof(char_t) // +1 for \0
		+ (selector.size() + 1) * sizeof(char_t) // +1 for \0
		+ returns * sizeof(ParamReflection*)
		+ params * sizeof(ParamReflection*);
}

MethodReflection* ClassLoader::createMethod(string name, string selector, uint32 staticSize, uint16 childrenCount, uint16 returns, uint16 params)
{
	uint32 dynamicSize = countMethodDynamicSize(name, selector, returns, params);

	MethodReflection* method = mClassHeap->alloc<MethodReflection>(staticSize + dynamicSize, childrenCount);
	method->mMaxStack = 100; // TODO: load
	initMethod(method, name, selector, staticSize, returns, params);
	
	//MethodInitializer* initializer = getMethodInitializer(selector);
	//if(initializer) initializer->initMethod(mVM, method);

	return method;
}

void ClassLoader::initMethod(MethodReflection* method, string name, string selector, uint32 staticSize, uint16 returns, uint16 params)
{
	byte* ptr = reinterpret_cast<byte*>(method) + staticSize; // move ptr at the end of static-sized object part
	
	// init flags
	method->mFlags = 0;

	// init name
	method->mNameCount = name.size() + 1; // +1 for \0
	method->mName = reinterpret_cast<char_t*>(ptr);
	memcpy(method->mName, name.c_str(), method->mNameCount * sizeof(char_t)); // c_str is terminated with \0

	ptr += method->mNameCount * sizeof(char_t); // move ptr after name

	// init selector
	method->mSelectorCount = selector.size() + 1; // +1 for \0
	method->mSelector = reinterpret_cast<char_t*>(ptr);
	memcpy(method->mSelector, selector.c_str(), method->mSelectorCount * sizeof(char_t)); // c_str is terminated with \0

	ptr += method->mSelectorCount * sizeof(char_t); // move ptr after selector

	// init returns
	method->mReturnsCount = returns;
	method->mReturns = reinterpret_cast<ParamReflection**>(ptr);
	for(uint16 i = 0; i < method->mReturnsCount; i++)
	{
		method->mReturns[i] = NULL;
	}

	ptr += method->mReturnsCount * sizeof(ParamReflection*); // move ptr after returns

	// init params
	method->mParamsCount = params;
	method->mParams = reinterpret_cast<ParamReflection**>(ptr);
	for(uint16 i = 0; i < method->mParamsCount; i++)
	{
		method->mParams[i] = NULL;
	}

	ptr += method->mParamsCount * sizeof(ParamReflection*); // move ptr after params
}

//////////////////// ---------- params ---------- ////////////////////

uint32 ClassLoader::countParamDynamicSize(string name)
{
	return (name.size() + 1) * sizeof(char_t) // +1 for \0
		+ 1 * sizeof(ClassReflection*); // type
}

ParamReflection* ClassLoader::createParam(string name, uint32 staticSize, uint16 childrenCount)
{
	uint32 dynamicSize = countParamDynamicSize(name);

	ParamReflection* param = mClassHeap->alloc<ParamReflection>(staticSize + dynamicSize, childrenCount);
	initParam(param, name, staticSize);
	
	//ParamInitializer* initializer = getParamInitializer(name);
	//if(initializer) initializer->initParam(mVM, param);

	return param;
}

void ClassLoader::initParam(ParamReflection* param, string name, uint32 staticSize)
{
	byte* ptr = reinterpret_cast<byte*>(param) + staticSize; // move ptr at the end of static-sized object part
	
	// init name
	param->mNameCount = name.size() + 1; // +1 for \0
	param->mName = reinterpret_cast<char_t*>(ptr);
	memcpy(param->mName, name.c_str(), param->mNameCount * sizeof(char_t)); // c_str is terminated with \0

	ptr += param->mNameCount * sizeof(char_t); // move ptr after nam

	// init type
	param->mType = NULL;
}