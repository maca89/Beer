#include "stdafx.h"
#include "LoadedObject.h"

#include "VirtualMachine.h"

#include "ClassFileDescriptor.h"
#include "ClassDescriptor.h"
#include "MethodDescriptor.h"
#include "AttributeDescriptor.h"
#include "ParamDescriptor.h"
#include "StringDescriptor.h"
#include "BytecodeDescriptor.h"

#include "PropertyReflection.h"
#include "ParamReflection.h"
#include "MethodReflection.h"

#include "Bytecode.h"

using namespace Beer;


ClassReflection* LoadedObjectInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, std::string name)
{
	return loader->createClass<LoadedObjectClass>(
		name, // classDescr->getName(classFile)->c_str()
		mClassDescr->getParentsLength() + 1, // + 1 for Object
		mClassDescr->getAttributesLength(), 
		mClassDescr->getMethodsLength()
	);
}

void LoadedObjectInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass) // TODO: add parents properties into properties size
{
	mClass = klass; // ugly

	uint16 propStart = 0;
	uint16 parentStart = 0;

	// default class
	ClassReflection* objectClass = vm->getObjectClass(); // class will be loaded if its not
	mClass->extends(parentStart++, objectClass);
	propStart += objectClass->getPropertiesCount();


	// parents
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		ClassReflection* parent = vm->getClass(getParentClassName(i));
		mClass->extends(parentStart + i, parent); // class will be loaded if its not
		propStart += objectClass->getPropertiesCount();
	}
			
	// attributes
	for(uint16 i = 0; i < mClassDescr->getAttributesLength(); i++)
	{
		AttributeDescriptor* attrDescr = getAtribute(i);
		PropertyReflection* prop = makeProperty(vm, attrDescr);
		mClass->setProperty(propStart + i, prop);
	}

	// methods
	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		MethodDescriptor* methodDescr = getMethod(i);
		MethodReflection* method = makeMethod(vm, loader, methodDescr);
		mClass->setMethod(i, method);
	}
}

AttributeDescriptor* LoadedObjectInitializer::getAtribute(uint16 i)
{
	return mClassFile->getDescriptor<AttributeDescriptor>(mClassDescr->getAttributeId(i));
}

MethodDescriptor* LoadedObjectInitializer::getMethod(uint16 i)
{
	return mClassFile->getDescriptor<MethodDescriptor>(mClassDescr->getMethodId(i));
}

PropertyReflection* LoadedObjectInitializer::makeProperty(VirtualMachine* vm, AttributeDescriptor* attrDescr)
{
	PropertyReflection* prop = new PropertyReflection;
			
	// set name
	prop->setName(attrDescr->getName(mClassFile)->c_str());

	// set type
	prop->setType(vm->getClass(mClassFile->getClassName(attrDescr->getTypeId())->c_str()));
			
	return prop;
}

MethodReflection* LoadedObjectInitializer::makeMethod(VirtualMachine* vm, ClassLoader* loader, MethodDescriptor* methodDescr)
{
	std::string name = methodDescr->getName(mClassFile)->c_str();
	
	// TODO - load
	std::string selector = std::string(mClass->getName()) + "::" + name;
	selector += "(";
	for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
	{
		ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
		selector += mClassFile->getClassName(paramDescr->getTypeId())->c_str();
		if(parami < methodDescr->getParamsLength() - 1) selector += ",";
	}
	selector += ")";
	
	MethodReflection* method = loader->createMethod<MethodReflection>(name, selector, methodDescr->getReturnsLength(), methodDescr->getParamsLength());
	
	method->mMaxStack = 20; // TODO

	// returns
	for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
	{
		ParamDescriptor* returnDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getReturnId(reti));
		
		ParamReflection* ret = loader->createParam<ParamReflection>(returnDescr->getName(mClassFile)->c_str());
		ret->setType(getType(mClassFile->getClassName(returnDescr->getTypeId()), vm));
		
		method->setReturn(reti, ret);
	}

	// params
	for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
	{
		ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
		
		ParamReflection* param = loader->createParam<ParamReflection>(paramDescr->getName(mClassFile)->c_str());
		param->setType(getType(mClassFile->getClassName(paramDescr->getTypeId()), vm));

		method->setParam(parami, param);
	}

	// bytecode
	BytecodeDescriptor* bcDescr = mClassFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
	Bytecode* bytecode = new Bytecode(bcDescr->getData(), bcDescr->getSize(), bcDescr->getInstrCount());
	bytecode->build(vm, mClassFile);
	method->setBytecode(bytecode);
	
	return method;
}

const char* LoadedObjectInitializer::getParentClassName(uint16 i)
{
	return mClassFile->getClassName(mClassDescr->getParentId(i))->c_str();
}

ClassReflection* LoadedObjectInitializer::getType(const StringDescriptor* name, VirtualMachine* vm)
{
	if(strcmp(mClass->getName(), name->c_str()) == 0)
	{
		return mClass; // prevents circular reference exception
	}

	return vm->getClass(name->c_str());
}