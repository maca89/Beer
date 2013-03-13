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
	BEER_STACK_CHECK();

	Class::createInstance(thread, receiver, ret); // call super method

	//return;

	/////////////////////////////////////////////////////////
	// fix missing value types
	StackRef<Integer> index(frame, frame->stackPush());
	StackRef<Property> prop(frame, frame->stackPush());
	StackRef<Class> klass(frame, frame->stackPush());

	for(uint32 i = 0; i < receiver->getPropertiesCount(); i++)
	{
		thread->createInteger(index, i);
		Class::getProperty(thread, receiver, index, prop);
		
		if(!prop.isNull())
		{
			Property::getPropertyType(thread, prop, klass);

			if(klass->isValueType())
			{
				StackRef<Object> child(frame, frame->stackPush()); // push child
				StackRef<Class> klassCopy(klass.push(frame)); // push class
				thread->createInstance(klassCopy, child); // pops copied class

				Object::setChild(thread, ret, child, Object::OBJECT_CHILDREN_COUNT + i);
				frame->stackPop(); // pop child
			}
		}
	}

	frame->stackMoveTop(-3); // pop index, prop, class
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
	StackFrame* frame = thread->getStackFrame();

	BEER_STACK_CHECK();

	StackRef<Method> method(frame, frame->stackPush());
	mClass = klass; // ugly, TODO: get rid of

	uint16 methodStart = 0;

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

		StackRef<String> str(frame, frame->stackPush());
		Method::getName(thread, method, str);

		string selector = string() + BEER_WIDEN("::") + str->c_str();
		selector += BEER_WIDEN("(");
		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			selector += String::gTranslate(vm, mClassFile->getClassName(paramDescr->getTypeId())->c_str())->c_str();
			if(parami < methodDescr->getParamsLength() - 1) selector += BEER_WIDEN(",");
		}
		selector += BEER_WIDEN(")");

		loader->addMethod(klass, *method, (string(className->c_str()) + selector).c_str());

		if(methodDescr->isOverride())
		{
			string interf = methodDescr->getInterfaceName(mClassFile)->c_str();
			loader->addMethod(klass, *method, (interf + selector).c_str());
		}

		frame->stackMoveTop(-1); // pop str
	}

	/*if(strcmp(className->c_str(), BEER_WIDEN("Task")) == 0)
	{
		loader->addMethod(klass, BEER_WIDEN("start"), BEER_WIDEN("Task::start()"), &Task::start, 0, 0);
	}*/

	loader->addMethod(klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &LoadedObject::createInstance, 1, 0);

	frame->stackMoveTop(-3); // pop class, classname, method
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
	Thread* thread = vm;
	StackFrame* frame = thread->getStackFrame();
	BEER_STACK_CHECK();

	StackRef<Property> prop(frame, frame->stackPush(
		loader->createProperty()
	));
	
	StackRef<String> name(frame, frame->stackPush(
		getString(vm, attrDescr->getName(mClassFile))
	));

	Property::setName(thread, prop, name);

	StackRef<Class> klass(frame, frame->stackPush(
		vm->getClass(getString(vm, mClassFile->getClassName(attrDescr->getTypeId())))
	));

	Property::setPropertyType(thread, prop, klass);
	
	Property* result = *prop; // ugly
	frame->stackMoveTop(-3); // pop prop, name, type
	return result;
}

Param* LoadedObjectInitializer::makeParam(VirtualMachine* vm, ClassLoader* loader, ParamDescriptor* paramDescr)
{
	Thread* thread = vm; // TODO: pass as argument
	StackFrame* frame = vm->getStackFrame();
	BEER_STACK_CHECK();

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
		thread->getClass(name)
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
	Thread* thread = vm; // TODO: pass as argument
	StackFrame* frame = thread->getStackFrame();

	BEER_STACK_CHECK();

	Reference<String> name = String::gTranslate(vm, methodDescr->getName(mClassFile)->c_str());

	StackRef<Method> method(frame, frame->stackPush(
		loader->createMethod(NULL, methodDescr->getReturnsLength(), methodDescr->getParamsLength())
	));

	StackRef<Integer> index(frame, frame->stackPush());

	// set name
	{
		StackRef<String> nameOnStack(frame, frame->stackPush(*name));
		Method::setName(thread, method, nameOnStack);
		frame->stackMoveTop(-1); // pop name
	}
	
	method->setMaxStack(20); // TODO: methodDescr->getMaxStack()

	// returns
	{
		StackRef<Param> param(frame, frame->stackPush());

		for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
		{
			thread->createInteger(index, reti);

			ParamDescriptor* returnDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getReturnId(reti));
			param = makeParam(vm, loader, returnDescr);

			Method::setReturn(thread, method, index, param);
		}

		frame->stackMoveTop(-1); // pop param
	}

	// params
	{
		StackRef<Param> param(frame, frame->stackPush());

		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			thread->createInteger(index, parami);

			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			param = makeParam(vm, loader, paramDescr);

			Method::setParam(thread, method, index, param);
		}

		frame->stackMoveTop(-1); // pop param
	}

	Method* result = *method; // ugly

	if(!methodDescr->isAbstract() && !methodDescr->isNative())
	{
		// bytecode
		BytecodeDescriptor* bcDescr = mClassFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
		Bytecode* bytecode = new Bytecode(bcDescr->getData(), bcDescr->getSize(), bcDescr->getInstrCount());
		bytecode->build(vm, result, mClassFile);
		method->setBytecode(bytecode);
	}


	frame->stackMoveTop(-2); // pop method, index
	return result;
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
		BEER_STACK_CHECK();

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