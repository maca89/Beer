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

void BEER_CALL LoadedObject::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret)
{
	StackFrame* frame = thread->getStackFrame();
	Class::createInstance(thread, receiver, ret); // call super method

	/////////////////////////////////////////////////////////
	// fix missing value types
	StackRef<Integer> index(frame, frame->stackPush());
	StackRef<Property> prop(frame, frame->stackPush());

	for(uint32 i = 0; i < receiver->getPropertiesCount(); i++)
	{
		thread->createInteger(index, i);
		Class::getProperty(thread, receiver, index, prop);
		
		if(!prop.isNull())
		{
			StackRef<Class> propType(frame, frame->stackPush(prop->getType()));
			if(propType->isValueType())
			{
				StackRef<Object> child(frame, frame->stackPush()); // push child
				StackRef<Class> klass(propType.push(frame)); // push class
				thread->createInstance(klass, child); // pops class

				Object::setChild(thread, ret, child, Object::OBJECT_CHILDREN_COUNT + i);
				frame->stackPop(); // pop child
			}
			frame->stackPop(); // pop type
		}
	}

	frame->stackMoveTop(-2); // pop index, prop
	/////////////////////////////////////////////////////////
}


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
		methodsLength + 1 // +1 for createInstance
	);
}

void LoadedObjectInitializer::initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass) // TODO: add parents properties into properties size
{
	Thread* thread = (Thread*)vm; // TODO: pass as argument
	Method* method = NULL;
	mClass = klass; // ugly

	uint16 methodStart = 0;

	StackFrame* frame = vm->getStackFrame();

	StackRef<Class> klassOnStack(frame, frame->stackPush(klass)); // TODO: pass as reference
	StackRef<String> className(frame, frame->stackPush());
	
	Class::getName(vm, klassOnStack, className);

	if(mClassDescr->getParentsLength() == 0)
	{
		Class* objectClass = vm->getObjectClass(); // class will be loaded if its not
		loader->extendClass(klass, objectClass);
	}

	// parents
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		Class* parent = vm->getClass(getParentClassName(vm, i)); // class will be loaded if its not
		loader->extendClass(klass, parent);
		//propStart += parent->getPropertiesCount();
	}
	
	// attributes
	{
		StackRef<Property> prop(frame, frame->stackPush());
		
		for(uint16 i = 0; i < mClassDescr->getAttributesLength(); i++)
		{
			AttributeDescriptor* attrDescr = getAtribute(i);
			prop = makeProperty(vm, loader, attrDescr);
			Class::addProperty(vm, klassOnStack, prop);
		}

		frame->stackMoveTop(-1); // pop property
	}

	// methods
	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		MethodDescriptor* methodDescr = getMethod(i);
		method = makeMethod(vm, loader, methodDescr);

		string selector = string() + BEER_WIDEN("::") + method->getName()->c_str();
		selector += BEER_WIDEN("(");
		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			selector += String::gTranslate(vm, mClassFile->getClassName(paramDescr->getTypeId())->c_str())->c_str();
			if(parami < methodDescr->getParamsLength() - 1) selector += BEER_WIDEN(",");
		}
		selector += BEER_WIDEN(")");

		loader->addMethod(klass, method, (string(className->c_str()) + selector).c_str());

		if(methodDescr->isOverride())
		{
			string interf = methodDescr->getInterfaceName(mClassFile)->c_str();
			loader->addMethod(klass, method, (interf + selector).c_str());
		}
	}

	if(strcmp(className->c_str(), BEER_WIDEN("Task")) == 0)
	{
		loader->addMethod(klass, BEER_WIDEN("dorun"), BEER_WIDEN("Task::dorun()"), &Task::dorun, 0, 0);
	}

	loader->addMethod(klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &LoadedObject::createInstance, 1, 0);

	frame->stackMoveTop(-2); // pop class, classname
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
	StackFrame* frame = vm->getStackFrame();
	StackRef<Param> param(frame, frame->stackPush(loader->createParam()));

	// get name
	StackRef<String> name(frame, frame->stackPush(
		getString(vm, paramDescr->getName(mClassFile))
	));

	// store name
	Param::setName(vm, param, name);
	
	// get classname
	name = getString(vm, mClassFile->getClassName(paramDescr->getTypeId()));

	// get class
	StackRef<Class> type(frame, frame->stackPush(
		vm->getClass(name)
	));

	// store class
	Param::setParamType(vm, param, type);
	
	Param* p = *param; // ugly, TODO

	// pop class & name & param
	frame->stackMoveTop(-3);

	return p;
}

Method* LoadedObjectInitializer::makeMethod(VirtualMachine* vm, ClassLoader* loader, MethodDescriptor* methodDescr)
{
	string name = String::gTranslate(vm, methodDescr->getName(mClassFile)->c_str())->c_str();
	Method* method = loader->createMethod(NULL, methodDescr->getReturnsLength(), methodDescr->getParamsLength());
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
	String* className = NULL;
	
	// fetch class name, veeeeery ugly, TODO!!!
	{
		StackFrame* frame = vm->getStackFrame();
		StackRef<Class> klassOnStack(frame, frame->stackPush(mClass)); // TODO: pass as reference
		StackRef<String> classNameOnStack(frame, frame->stackPush());
		Class::getName(vm, klassOnStack, classNameOnStack);
		className = *classNameOnStack;
		frame->stackMoveTop(-2); // pop class & classname
	}

	String* strname = getString(vm, name);
	if(className->compare(strname) == 0)
	{
		return mClass; // prevents circular reference exception
	}

	return vm->getClass(strname->c_str());
}