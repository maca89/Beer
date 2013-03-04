#include "stdafx.h"
#include "LoadedObject.h"

#include "VirtualMachine.h"

#include "ClassFileDescriptor.h"
#include "ClassDescriptor.h"
#include "MethodDescriptor.h"
#include "AttributeDescriptor.h"
#include "ParamDescriptor.h"
//#include "StringDescriptor.h"
#include "BytecodeDescriptor.h"

#include "Property.h"
#include "Param.h"
#include "Method.h"

#include "Bytecode.h"
#include "Task.h"

using namespace Beer;


Class* LoadedObjectInitializer::createClass(VirtualMachine* vm, ClassLoader* loader, String* name)
{
	string className = name->c_str();

	uint16 methodsLength = mClassDescr->getMethodsLength();
	uint16 propertiesLength = mClassDescr->getAttributesLength();

	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		MethodDescriptor* methodDescr = getMethod(i);
		if(methodDescr->isOverride())
		{
			methodsLength += 1; // will have to save two times with two selectors
		}
	}

	if(strcmp(name->c_str(), BEER_WIDEN("Task")) == 0)
	{
		methodsLength++; // dorun
	}

	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		Class* parent = vm->getClass(getParentClassName(vm, i));
		propertiesLength += parent->getPropertiesCount();
	}

	return loader->createClass<Class>(
		name, // classDescr->getName(classFile)->c_str()
		mClassDescr->getParentsLength() == 0 ? 1 : mClassDescr->getParentsLength() /*+ (name.compare(BEER_WIDEN("Main")) == 0 ? 1 : 0)*/, // + 1 for Object, TODO: Main in classfile
		propertiesLength, 
		methodsLength
	);
}

void LoadedObjectInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass) // TODO: add parents properties into properties size
{
	mClass = klass; // ugly

	uint16 propStart = 0;
	uint16 parentStart = 0;
	uint16 methodStart = 0;
	
	// Task class, TODO: in classfile
	//if(strcmp(klass->getName(), BEER_WIDEN("Main")) == 0)
	//{
		//ClassReflection* taskClass = vm->getClass(BEER_WIDEN("Task"));
		//mClass->extends(parentStart++, taskClass); // class will be loaded if its not
		//propStart += taskClass->getPropertiesCount();
	//}

	// default Object class, TODO: in classfile
	//else
	if(mClassDescr->getParentsLength() == 0)
	{
		Class* objectClass = vm->getObjectClass(); // class will be loaded if its not
		mClass->extends(parentStart++, objectClass);
		propStart += objectClass->getPropertiesCount();
	}

	// parents
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		Class* parent = vm->getClass(getParentClassName(vm, i));
		mClass->extends(parentStart + i, parent); // class will be loaded if its not
		propStart += parent->getPropertiesCount();
	}
			
	// attributes
	for(uint16 i = 0; i < mClassDescr->getAttributesLength(); i++)
	{
		AttributeDescriptor* attrDescr = getAtribute(i);
		Property* prop = makeProperty(vm, loader, attrDescr);
		mClass->setProperty(propStart + i, prop);
	}

	// methods
	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		MethodDescriptor* methodDescr = getMethod(i);
		Method* method = NULL;
		
		/*dif(methodDescr->isNative())
		{
			if(strcmp(klass->getName(), BEER_WIDEN("Task")) == 0 && strcmp(methodDescr->getName(mClassFile)->c_str(), BEER_WIDEN("dorun")) == 0)
			{
				method = loader->createMethod<MethodReflection>(BEER_WIDEN("dorun"), 0, 0);
				method->setFunction(&TaskClass::dorun);
				klass->setMethod(i, vm->createPair(vm->createString(BEER_WIDEN("Task::dorun()")), method));
			}
			else
			{
				throw ClassFileException((string(BEER_WIDEN("Unknown native method ")) + methodDescr->getName(mClassFile)->c_str() + BEER_WIDEN(" for class ") + klass->getName()).c_str());
			}
		}*/
		//else
		//{
			method = makeMethod(vm, loader, methodDescr);
		//}

		string selector = string() + BEER_WIDEN("::") + method->getName()->c_str();
		selector += BEER_WIDEN("(");
		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			selector += String::gTranslate(vm, mClassFile->getClassName(paramDescr->getTypeId())->c_str())->c_str();
			if(parami < methodDescr->getParamsLength() - 1) selector += BEER_WIDEN(",");
		}
		selector += BEER_WIDEN(")");

		mClass->setMethod(methodStart++, vm->createPair(vm->createString((string(mClass->getName()->c_str()) + selector)), method)); // TODO

		if(methodDescr->isOverride())
		{
			bool abstra = methodDescr->isAbstract();
			string interf = methodDescr->getInterfaceName(mClassFile)->c_str();
			string interfSelector = interf + selector;
			mClass->setMethod(methodStart++, vm->createPair(vm->createString(interfSelector.c_str()), method));
		}
	}

	if(strcmp(klass->getName()->c_str(), BEER_WIDEN("Task")) == 0)
	{
		Method* method = loader->createMethod(0, 0);
		method->setName(vm->createString(BEER_WIDEN("dorun")));
		method->setFunction(&Task::dorun);
		klass->setMethod(mClassDescr->getMethodsLength(), vm->createPair(vm->createString(BEER_WIDEN("Task::dorun()")), method));
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

Property* LoadedObjectInitializer::makeProperty(VirtualMachine* vm, ClassLoader* loader, AttributeDescriptor* attrDescr)
{
	Property* prop = loader->createProperty();
	
	prop->setName(getString(vm, attrDescr->getName(mClassFile)));
	prop->setType(vm->getClass(getString(vm, mClassFile->getClassName(attrDescr->getTypeId()))));
			
	return prop;
}

Param* LoadedObjectInitializer::makeParam(VirtualMachine* vm, ClassLoader* loader, ParamDescriptor* paramDescr)
{
	Param* param = loader->createParam();

	param->setName(getString(vm, paramDescr->getName(mClassFile)));
	param->setType(vm->getClass(getString(vm, mClassFile->getClassName(paramDescr->getTypeId()))));
			
	return param;
}

Method* LoadedObjectInitializer::makeMethod(VirtualMachine* vm, ClassLoader* loader, MethodDescriptor* methodDescr)
{
	string name = String::gTranslate(vm, methodDescr->getName(mClassFile)->c_str())->c_str();
	Method* method = loader->createMethod(methodDescr->getReturnsLength(), methodDescr->getParamsLength());
	method->setName(vm->createString(name));
	method->setMaxStack(20); // TODO: methodDescr->getMaxStack()

	// returns
	for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
	{
		ParamDescriptor* returnDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getReturnId(reti));
		Param* ret = makeParam(vm, loader, returnDescr);
		method->setReturn(reti, ret);
	}

	// params
	for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
	{
		ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
		Param* param = makeParam(vm, loader, paramDescr);
		method->setParam(parami, param);
	}

	if(!methodDescr->isAbstract() && !methodDescr->isNative())
	{
		// bytecode
		BytecodeDescriptor* bcDescr = mClassFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
		Bytecode* bytecode = new Bytecode(bcDescr->getData(), bcDescr->getSize(), bcDescr->getInstrCount());
		bytecode->build(vm, mClassFile);
		method->setBytecode(bytecode);
	}
	
	return method;
}

const char_t* LoadedObjectInitializer::getParentClassName(VirtualMachine* vm, uint16 i)
{
	return String::gTranslate(vm, mClassFile->getClassName(mClassDescr->getParentId(i))->c_str())->c_str();
}

Class* LoadedObjectInitializer::getType(const StringDescriptor* name, VirtualMachine* vm)
{
	String* strname = getString(vm, name);
	if(mClass->getName()->compare(strname) == 0)
	{
		return mClass; // prevents circular reference exception
	}

	return vm->getClass(strname->c_str());
}