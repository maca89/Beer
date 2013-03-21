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
#include "BytecodeBuilder.h"

#include "Property.h"
#include "Param.h"
#include "Method.h"

#include "Bytecode.h"
#include "Task.h"

using namespace Beer;

void BEER_CALL LoadedObject::createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	Class::createInstance(thread, receiver, ret); // call super method

	/////////////////////////////////////////////////////////
	// fix missing value types
	if(false) {
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Property> prop(frame, frame->stackPush());
		StackRef<Class> klass(frame, frame->stackPush());

		StackRef<Integer> propertiesCount(frame, frame->stackPush());
		Class::getPropertiesCount(thread, receiver, propertiesCount);

		for(uint32 i = 0; i < propertiesCount->getData(); i++)
		{
			thread->createInteger(index, i);
			Class::getProperty(thread, receiver, index, prop);
		
			if(!prop.isNull())
			{
				Property::getPropertyType(thread, prop, klass);

				if(klass->isValueType())
				{
					StackRef<Object> child(frame, frame->stackPush()); // push child
					thread->createInstance(klass, child);

					Object::setChild(thread, ret, Object::OBJECT_CHILDREN_COUNT + i, child);
					frame->stackPop(); // pop child
				}
			}
		}

		frame->stackMoveTop(-4); // pop index, prop, class, propertiesCount
	}
	/////////////////////////////////////////////////////////
}


void LoadedObjectInitializer::createClass(Thread* thread, ClassLoader* loader, StackRef<String> name, StackRef<Class> ret)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

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

	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		StackRef<String> parentClassName(frame, frame->stackPush());
		getParentClassName(thread, parentClassName, i);

		StackRef<Class> parent(frame, frame->stackPush());
		thread->findClass(parentClassName, parent);

		StackRef<Integer> propertiesCount(frame, frame->stackPush());
		Class::getPropertiesCount(thread, parent, propertiesCount);

		StackRef<Integer> methodsCount(frame, frame->stackPush());
		Class::getMethodsCount(thread, parent, methodsCount);

		propertiesLength += static_cast<uint16>(propertiesCount->getData());
		methodsLength += static_cast<uint16>(methodsCount->getData());

		frame->stackMoveTop(-4); // pop parentClassName, parent, propertiesCount, methodsCount
	}

	if(mClassDescr->getParentsLength() == 0)
	{
		StackRef<Class> objectKlass(frame, frame->stackPush());
		thread->getObjectClass(objectKlass);

		StackRef<Integer> methodsCount(frame, frame->stackPush());
		Class::getMethodsCount(thread, objectKlass, methodsCount);

		methodsLength += static_cast<uint16>(methodsCount->getData());

		frame->stackMoveTop(-2); // pop objectKlass, methodsCount
	}

	loader->createClass<Class>(
		thread, 
		name, // classDescr->getName(classFile)->c_str()
		ret,
		mClassDescr->getParentsLength() == 0 ? 1 : mClassDescr->getParentsLength() /*+ (name.compare(BEER_WIDEN("Main")) == 0 ? 1 : 0)*/, // + 1 for Object, TODO: Main in classfile
		propertiesLength, 
		methodsLength + 1 // +1 for createInstance
	);
}

void LoadedObjectInitializer::initClass(Thread* thread, ClassLoader* loader, StackRef<Class> klass) // TODO: add parents properties into properties size
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	StackRef<Method> method(frame, frame->stackPush());
	mClass = *klass; // ugly, TODO: get rid of

	uint16 methodStart = 0;

	StackRef<String> className(frame, frame->stackPush());
	
	Class::getName(thread, klass, className);

	if(mClassDescr->getParentsLength() == 0)
	{
		StackRef<Class> objectClass(frame, frame->stackPush());
		thread->getObjectClass(objectClass);
		Class::addParent(thread, klass, objectClass);
		frame->stackPop(); // pop objectClass
	}

	// parents
	for(uint16 i = 0; i < mClassDescr->getParentsLength(); i++)
	{
		StackRef<String> parentClassName(frame, frame->stackPush());
		getParentClassName(thread, parentClassName, i);

		StackRef<Class> parentClass(frame, frame->stackPush());
		thread->findClass(parentClassName, parentClass);

		Class::addParent(thread, klass, parentClass);
		
		frame->stackMoveTop(-2); // pop parentClass, parentClassName
	}
	
	// attributes
	{
		StackRef<Property> prop(frame, frame->stackPush());
		
		for(uint16 i = 0; i < mClassDescr->getAttributesLength(); i++)
		{
			AttributeDescriptor* attrDescr = getAtribute(i);
			makeProperty(thread, prop, loader, attrDescr);
			Class::addProperty(thread, klass, prop);
		}

		frame->stackPop(); // pop property
	}

	// methods
	for(uint16 i = 0; i < mClassDescr->getMethodsLength(); i++)
	{
		MethodDescriptor* methodDescr = getMethod(i);
		makeMethod(thread, method, loader, methodDescr);

		StackRef<String> str(frame, frame->stackPush());
		Method::getName(thread, method, str);
		string selector = string() + BEER_WIDEN("::") + str->c_str();

		selector += BEER_WIDEN("(");
		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			selector += String::gTranslate(thread, mClassFile->getClassName(paramDescr->getTypeId())->c_str())->c_str();
			if(parami < methodDescr->getParamsLength() - 1) selector += BEER_WIDEN(",");
		}
		selector += BEER_WIDEN(")");

		loader->addMethod(thread, klass, method, (string(className->c_str()) + selector).c_str());

		if(methodDescr->isOverride())
		{
			string interf = methodDescr->getInterfaceName(mClassFile)->c_str();
			loader->addMethod(thread, klass, method, (interf + selector).c_str());
		}

		frame->stackPop(); // pop str
	}

	loader->addMethod(thread, klass, BEER_WIDEN("createInstance"), BEER_WIDEN("$Class::createInstance()"), &LoadedObject::createInstance, 1, 0);

	frame->stackMoveTop(-2); // pop classname, method
}

AttributeDescriptor* LoadedObjectInitializer::getAtribute(uint16 i)
{
	return mClassFile->getDescriptor<AttributeDescriptor>(mClassDescr->getAttributeId(i));
}

MethodDescriptor* LoadedObjectInitializer::getMethod(uint16 i)
{
	return mClassFile->getDescriptor<MethodDescriptor>(mClassDescr->getMethodId(i));
}

void LoadedObjectInitializer::makeProperty(Thread* thread, StackRef<Property> ret, ClassLoader* loader, AttributeDescriptor* attrDescr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	loader->createProperty(thread, ret);
	
	// set name
	{
		StackRef<String> name(frame, frame->stackPush());
		getString(thread, name, attrDescr->getName(mClassFile));

		Property::setPropertyName(thread, ret, name);
		frame->stackPop(); // pop name
	}

	// set class
	{
		StackRef<Class> klass(frame, frame->stackPush());
	
		// fetch klass
		{
			StackRef<String> klassName(frame, frame->stackPush());
			getString(thread, klassName, mClassFile->getClassName(attrDescr->getTypeId()));

			thread->findClass(klassName, klass);
			frame->stackPop(); // pop klassName
		}

		Property::setPropertyType(thread, ret, klass);
		frame->stackPop(); // pop klass
	}
}

void LoadedObjectInitializer::makeParam(Thread* thread, StackRef<Param> ret, ClassLoader* loader, ParamDescriptor* paramDescr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	loader->createParam(thread, ret);

	// set name
	{
		StackRef<String> name(frame, frame->stackPush());
		getString(thread, name, paramDescr->getName(mClassFile));

		Param::setName(thread, ret, name);
		frame->stackPop(); // pop name
	}
	
	// set class
	{
		StackRef<String> klassName(frame, frame->stackPush());
		getString(thread, klassName, mClassFile->getClassName(paramDescr->getTypeId()));

		StackRef<Class> klass(frame, frame->stackPush());
		thread->findClass(klassName, klass);

		Param::setParamType(thread, ret, klass);
		frame->stackMoveTop(-2); // pop klassName, klass
	}
}

void LoadedObjectInitializer::makeMethod(Thread* thread, StackRef<Method> ret, ClassLoader* loader, MethodDescriptor* methodDescr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	loader->createMethod(thread, ret, NULL, methodDescr->getReturnsLength(), methodDescr->getParamsLength());

	// set name
	{
		StackRef<String> name(frame, frame->stackPush(
			*String::gTranslate(thread, methodDescr->getName(mClassFile)->c_str())
		));

		Method::setName(thread, ret, name);
		frame->stackPop(); // pop name
	}
	
	ret->setMaxStack(20); // TODO: methodDescr->getMaxStack()

	// returns
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Param> param(frame, frame->stackPush());

		for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
		{
			thread->createInteger(index, reti);

			ParamDescriptor* returnDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getReturnId(reti));
			makeParam(thread, param, loader, returnDescr);

			Method::setReturn(thread, ret, index, param);
		}

		frame->stackMoveTop(-2); // pop param, index
	}

	// params
	{
		StackRef<Integer> index(frame, frame->stackPush());
		StackRef<Param> param(frame, frame->stackPush());

		for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
		{
			thread->createInteger(index, parami);

			ParamDescriptor* paramDescr = mClassFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
			makeParam(thread, param, loader, paramDescr);

			Method::setParam(thread, ret, index, param);
		}
		
		frame->stackMoveTop(-2); // pop param, index
	}

	if(!methodDescr->isAbstract() && !methodDescr->isNative())
	{
		// bytecode
		BytecodeDescriptor* bcDescr = mClassFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
		thread->getVM()->getBytecodeBuilder()->build(thread, ret, mClassFile, bcDescr);
	}
}

void LoadedObjectInitializer::getParentClassName(Thread* thread, StackRef<String> ret, uint16 i)
{
	ret = *String::gTranslate(thread, mClassFile->getClassName(mClassDescr->getParentId(i))->c_str());
}

const char_t* LoadedObjectInitializer::getParentClassName(Thread* thread, uint16 i)
{
	return String::gTranslate(thread, mClassFile->getClassName(mClassDescr->getParentId(i))->c_str())->c_str();
}

void LoadedObjectInitializer::getType(Thread* thread, StackRef<Class> ret, const StringDescriptor* name)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();
	
	StackRef<String> thisClassName(frame, frame->stackPush());
	
	// fetch class name, veeeeery ugly, TODO!!!
	{
		StackRef<Class> klassOnStack(frame, frame->stackPush(mClass)); // TODO: pass as reference
		Class::getName(thread, klassOnStack, thisClassName);
		frame->stackPop(); // pop class
	}
	
	StackRef<String> thatClassName(frame, frame->stackPush());
	getString(thread, thatClassName, name);
	
	if(thisClassName->compare(*thatClassName) == 0)
	{
		ret = mClass; // prevents circular reference exception
	}
	else
	{
		thread->findClass(thatClassName, ret);
	}

	frame->stackMoveTop(-2); // pop thisClassName, thatClassName
}